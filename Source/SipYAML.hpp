/*!
 *	Copyright (c) 2013 Daniel Hrabovcak
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to
 *	deal in the Software without restriction, including without limitation the
 *	rights to use, copy, modify, merge, publish, distribute, sub-license, and/or
 *	sell copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *	
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *	
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *	IN THE SOFTWARE.
**/
#ifndef SIPYAML__H_eTNcyHjx
#define SIPYAML__H_eTNcyHjx
#include <memory>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

#include <stack>
using std::stack;

// DELETE
#include <iostream>
using std::cout;

/*!
 *	Define this macro before including this header file to set the size of the
 *	static memory pool for nodes. All data is allocated internally from the pool
 *	using placement new before allocating data dynamically. This size is in
 *	bytes and must be a real number greater than or equal to 0.
**/
#ifndef SIPYAML_STATIC_POOL_SIZE
#define SIPYAML_STATIC_POOL_SIZE 64 * 1024
#elif (!(SIPYAML_STATIC_POOL_SIZE >= 0))
#error	"SIPYAML_STATIC_POOL_SIZE must be a value greater than or equal 0."
#endif // SIPYAML_STATIC_POOL_SIZE
/*!
 *	Define this macro before including this header file to set the size of the
 *	dynamic memory pool for nodes. The dynamic memory is used after the static
 *	memory pool has run out of space. This size must be greater than the size
 *	of the a node plus the size of a pointer, or else the possibility of a
 *	buffer is high.
**/
#ifndef SIPYAML_DYNAMIC_POOL_SIZE
#define SIPYAML_DYNAMIC_POOL_SIZE 1024
#elif (SIPYAML_DYNAMIC_POOL_SIZE < 128)
#error "SIPYAML_DYNAMIC_POOL_SIZE must be greater than the node size."
#endif // SIPYAML_DYNAMIC_POOL_SIZE

// Check GCC flag.
#ifdef __BYTEORDER__
#if __BYTEORDER__ == __ORDER_BIG_ENDIAN__
/*!
 *	Define this macro before including this header file to enable big endian.
 *	By default, all classes are little endian unless this flag is defined.
 *	This is only necessary if UTF16 or UTF32 is being used.
**/
#define SIPYAML_BIG_ENDIAN
#endif
#endif // __BYTEORDER__

namespace Sip
{
	// Generic internal functions.
	namespace Unicode
	{
		/*!
		 *	Returns the length of the given data. A piece of data ends when a
		 *	null character is found.
		**/
		template <typename Char> size_t datalen(const Char *data)
		{
			const Char *first = data;
			while (*data)
			{
				++data;
			}
			return data - first;
		}

		/*!
		 *	Stores possible BOM types for YAML, which only supports UTF-8 and
		 *	UTF-16.
		**/
		enum BOM : char
		{
			UTF8,
			UTF16LE,
			UTF16BE,
			UTF32LE,
			UTF32BE
		};
	
		/*!
o		 *	Returns the BOM type of the indicated string. The string must be at
		 *	least 4 length long.
		**/
		BOM checkBOM(const char *bom)
		{
			// Order as specified in YAML 1.2 specification:
			if (bom[0] == 0x00 && bom[1] == 0x00 &&
				bom[2] == static_cast<char>(0xFE) &&
				bom[3] == static_cast<char>(0xFF))
			{
				return UTF32BE;
			}
			if (bom[0] == 0x00 && bom[1] == 0x00 && bom[2] == 0x00)
			{
				return UTF32BE;
			}

			if (bom[0] == static_cast<char>(0xFF) &&
				bom[1] == static_cast<char>(0xFE) &&
				bom[2] == 0x00 && bom[3] == 0x00)
			{
				return UTF32LE;
			}
			if (bom[1] == 0x00 && bom[2] == 0x00 && bom[3] == 0x00)
			{
				return UTF32LE;
			}
			
			if (bom[0] == static_cast<char>(0xFE) &&
				bom[1] == static_cast<char>(0xFF))
			{
				return UTF16BE;
			}
			if (bom[0] == 0x00)
			{
				return UTF16BE;
			}

			if (bom[0] == static_cast<char>(0xFF) &&
				bom[1] == static_cast<char>(0xFE))
			{
				return UTF16LE;
			}
			if (bom[1] == 0x00)
			{
				return UTF16LE;
			}

			if (bom[0] == static_cast<char>(0xEF) &&
				bom[1] == static_cast<char>(0xBB) &&
				bom[2] == static_cast<char>(0xBF))
			{
				return UTF8;
			}
			return UTF8;
		}

		struct CharUTF8
		{
			typedef char CharType;
			static inline bool isChar(CharType type, char ch)
			{
				return type == ch;
			}
		};

		struct CharUTF16
		{
			typedef int16_t CharType;
			static inline bool isChar(CharType type, char ch)
			{
				return type == ch;
			}
		};

		struct CharUTF16Inverse
		{
			typedef int16_t CharType;
			static inline bool isChar(CharType type, char ch)
			{
				return (type & 0xFF) == ch && (type & 0xFF00) == 0;
			}
		};
	}

	/*!
	 *	The type of node. Can be used as an alternative to reading the value.
	**/
	enum YAMLType : uint8_t
	{
		DocBegin	=	0,			/*!< Indicates The start of a document.
										There is no value or key. */
		DocEnd		=	1,			/*!< Indicates the end of a document. There
										is no value or key. */
		Directive	=	2,			
		MapElement	=	3,
		ListElement	=	4,
		Comment		=	5,
		IsAnchor	=	0x10,		//!< (Flag) This node is an anchor.
		IsReference	=	0x20,		//!< (Flag) This node is a reference.
		Block		=	0x00,		//!< (Flag) Child nodes appear as blocked.
		Flow		=	0x80		//!< (Flag) Child nodes appear in-line.
	};
	
	/*!
	 *	A generic node class that stores other nodes as children and siblings in
	 *	a linked list type format.
	**/
	template <typename NodeType> struct NodeBase
	{
		void appendNode(NodeType *node)
		{
			assert(node && !node->_parent);// &&
				//!node->_nextSibling && !node->_previousSibling);
			node->_parent = static_cast<NodeType*>(this);
			node->_nextSibling = nullptr;
			if (_firstChild)
			{
				_lastChild->_nextSibling = node;
				node->_previousSibling = _lastChild;
			}
			else
			{
				_firstChild = node;
			}
			_lastChild = node;
		}
		
		inline NodeType *parent() const
		{
			return _parent;
		}
		
		inline NodeType *nextSibling() const
		{
			return _nextSibling;
		}
		
		inline NodeType *firstChild() const
		{
			return _firstChild;
		}
		
		inline NodeType *lastChild() const
		{
			return _lastChild;
		}
	
	protected:
	
		NodeBase() : _parent(nullptr), _nextSibling(nullptr),
			_previousSibling(nullptr), _firstChild(nullptr),
			_lastChild(nullptr) {}
		
	private:
		NodeType *_parent;
		NodeType *_nextSibling;
		NodeType *_previousSibling;
		NodeType *_firstChild;
		NodeType *_lastChild;
	};

	/*!
	 *	A Node is a representation of data that has a child, parent and
	 *	siblings. Implemented as a linked list.
	**/
	template <typename Char> struct YAMLNode :
		public NodeBase<YAMLNode<Char>>
	{
		typedef typename Char::CharType CharType;
		
		YAMLNode() {}
		YAMLNode(YAMLType type, const CharType *key = 0, size_t keySize = 0,
			const CharType *value = 0, size_t valueSize = 0) :
			NodeBase<YAMLNode<Char>>(), _key(key), _value(value), _type(type) {}
		
		inline YAMLType type() const
		{
			return _type;
		}
		
		inline const CharType *key() const
		{
			return _key;
		}
		
		inline size_t keySize() const
		{
			return _keySize;
		}
		
		void setKey(const CharType *key)
		{
			_key = key;
			_keySize = Unicode::datalen(key);
		}
		
		void setKey(const CharType *key, size_t size)
		{
			_key = key;
			_keySize = size;
		}
		
		inline const CharType *value() const
		{
			return _value;
		}
		
		inline size_t valueSize() const
		{
			return _valueSize;
		}
		
		void setValue(const CharType *value)
		{
			_value = value;
			_valueSize = Unicode::datalen(value);
		}
		
		void setValue(const CharType *value, size_t size)
		{
			_value = value;
			_valueSize = size;
		}
		
	protected:
		const CharType *_key;
		const CharType *_value;
		size_t _keySize;
		size_t _valueSize;
		YAMLType _type;
	};
	
	namespace Internal
	{
		template <typename Char> void printYAMLChildren(
			std::string *printer, NodeBase<YAMLNode<Char>> *node,
			size_t indent = 0);
		
		/*!
		 *	Prints a YAML map element.
		**/
		template <typename Char> void printYAMLMap(std::string *printer,
			YAMLNode<Char> *node, size_t indent = 0)
		{
			if (!printer->empty())
			{
				printer->append(1, '\n');
			}
			printer->append(indent, ' ');
			printer->append(node->key(), node->keySize());
			printer->append(": ");
			if (node->value())
			{
				printer->append(node->value(), node->valueSize());
			}
			printYAMLChildren(printer, node, indent + 2);
		}
		
		/*!
		 *	Prints a YAML list element.
		**/
		template <typename Char> void printYAMLList(std::string *printer,
			YAMLNode<Char> *node, size_t indent = 0)
		{
			if (!printer->empty())
			{
				printer->append(1, '\n');
			}
			printer->append(indent, ' ');
			printer->append("- ");
			if (node->key())
			{
				printer->append(node->key(), node->keySize());
				printer->append(": ");
			}
			printer->append(node->value(), node->valueSize());
			printYAMLChildren(printer, node, indent + 2);
		}
		
		/*!
		 *	Prints a YAML comment.
		**/
		template <typename Char> void printYAMLComment(
			std::string *printer, YAMLNode<Char> *node, size_t indent = 0)
		{
			if (node->type() & YAMLType::Flow)
			{
				printer->append(" # ");
			}
			else
			{
				if (!printer->empty())
				{
					printer->append(1, '\n');
				}
				printer->append("# ");
			}
			printer->append(node->value(), node->valueSize());
		}
		
		/*!
		 *	Prints the children of a YAML node.
		**/
		template <typename Char> void printYAMLChildren(
			std::string *printer, NodeBase<YAMLNode<Char>> *node,
			size_t indent = 0)
		{
			YAMLNode<Char> *child = node->firstChild();
			while (child)
			{
				switch (child->type() & 0xF)
				{
				case YAMLType::DocBegin:
					printer->append("---");
					break;
				case YAMLType::DocEnd:
					printer->append("...");
					break;
				case YAMLType::MapElement:
					printYAMLMap(printer, child, indent);
					break;
				case YAMLType::ListElement:
					printYAMLList(printer, child, indent);
					break;
				case YAMLType::Directive:
					printer->append(1, '%');
					printer->append(child->key(), child->keySize());
					if (child->value())
					{
						printer->append(" ");
						printer->append(child->value(), child->valueSize());
					}
					break;
				}
				child = child->nextSibling();
			}
		}
	}

	/*!
	 *	A generic class that allocates data from a memory pool for a single node
	 *	type. Data is preallocated in bytes, with the total number of bytes
	 *	equalling the indicated StaticPoolSize multiplied by the Node size, as
	 *	stack memory. When it runs out, memory will begin to be placed on the
	 *	heap whose size is indicated by DynamicPoolSize.
	**/
	template <typename NodeType, size_t StaticPoolSize,
			  size_t DynamicPoolSize> struct MemoryPool
	{
		/*!
		 *	Creates a memory pool.
		**/
		MemoryPool() : _memoryFirst(0),
			_memoryPosition((char*)_memoryStatic),
			_memoryEnd(_memoryPosition + StaticPoolSize) {}
		
		/*!
		 *	Clears all internal data.
		**/
		~MemoryPool()
		{
			clear();
		}
		
	protected:
	
		/*!
		 *	Returns the next available free space, creating it if it does not
		 *	exist.
		**/
		void *allocate()
		{
			void *position = _memoryPosition;
			if (_memoryPosition + sizeof(NodeType) > _memoryEnd)
			{
				_memoryPosition = new char[64];
				memcpy(_memoryPosition, &_memoryFirst, sizeof(void*));
				_memoryFirst = _memoryPosition;
				_memoryEnd = _memoryPosition + 64;
				position = _memoryPosition + sizeof(void*);
			}
			_memoryPosition += sizeof(NodeType);
			return position;
		}
		
		/*!
		 *	Clears all stored dynamic memory blocks.
		**/
		void clear()
		{
			if (_memoryFirst)
			{
				void *location = _memoryFirst;
				while (location)
				{
					memcpy(&location, _memoryFirst, sizeof(void*));
					delete[] _memoryFirst;
					_memoryFirst = static_cast<char*>(location);
				}
			}
		}
		
	private:
		
		char _memoryStatic[StaticPoolSize];
		char *_memoryFirst;			// Current memory block first position.
		char *_memoryPosition;		// Free memory position.
		char *_memoryEnd;			// Memory not allowed to write.
	};
	
	/*!
	 *	Represents a YAML document.
	**/
	template <typename Char> struct YAMLDocument :
		public MemoryPool<YAMLNode<Char>, SIPYAML_STATIC_POOL_SIZE,
		SIPYAML_DYNAMIC_POOL_SIZE>, public NodeBase<YAMLNode<Char>>
	{
		typedef typename Char::CharType CharType;
		typedef YAMLNode<Char> Node;
	
		/*!
		 *	Creates and returns a new YAML node. This node is automatically
		 *	deleted when the document is deleted.
		**/
		YAMLNode<Char> *allocateNode(YAMLType type, const CharType *key = 0,
			size_t keySize = 0, const CharType *value = 0, size_t valueSize = 0)
		{
			YAMLNode<Char> *node = new(this->allocate()) YAMLNode<Char>(type, 
				key, keySize, value, valueSize);
			return node;
		}
		
		/*!
		 *	Prints a readable YAML file representation.
		**/
		void print(std::string *printer)
		{
			Internal::printYAMLChildren(printer, this);
		}
		
		/*!
		 *	Parses a YAML file.
		**/
		void parse(const CharType *yaml)
		{
			size_t position = 0;
			size_t indent =  0;
			YAMLNode<Char> *node;
			NodeBase<YAMLNode<Char>> *inserting = this;
			
			stack<size_t> indents;
			indents.push(0);
			
			while (1)
			{
				if (Char::isChar(yaml[position], '\0'))
				{
					break;
				}
				node = nullptr;
				indent = 0;
				while (Char::isChar(yaml[position], ' '))
				{
					indent += 1;
					++position;
				}

				if (Char::isChar(yaml[position], '-'))
				{
					if (Char::isChar(yaml[position + 1], '-') &&
						Char::isChar(yaml[position + 2], '-'))
					{
						node = allocateNode(Sip::DocBegin);
						position += 3;
					}
					else if (Char::isChar(yaml[position + 1], ' '))
					{
						// Read key/value.
						node = allocateNode(Sip::ListElement, 0, 0,
							&yaml[++position]);
						while (notEndKey(&yaml[++position])) {}
						// TODO: Check key size.
						if (isKeyChar(&yaml[position]))
						{
							// Both scalar and key.
							node->setKey(node->value(),
								&yaml[position] - node->value() - 1);
							node->setValue(&yaml[position], 0);
							while (notEnd(yaml[++position])) {}
							// TODO: Check value size.
							node->setValue(node->value(),
								&yaml[position] - node->value());
						}
						else
						{
							// Only scalar, no key.
							node->setValue(node->value(),
								&yaml[position] - node->value());
						}
					}
					else
					{
						// TODO: This should go parse maps.
					}
				}
				else if (!Char::isChar(yaml[position], '#'))
				{
					// Map Elements processing.
					node = allocateNode(Sip::MapElement, &yaml[position], 0);
					while (notEndKey(&yaml[++position])) {}
					if (!isKeyChar(&yaml[position]))
					{
						cout << "ERROR!";
						// TODO: Error, expected key.
					}
					node->setKey(node->key(),
						&yaml[position] - node->key() - 1);
					node->setValue(&yaml[++position], 0);
					while (notEnd(yaml[++position])) {}
					node->setValue(node->value(),
						&yaml[position] - node->value());
					// set value
				}
				else
				{
					// For comments.
					//node = this;
				}
				
				// Checks for comment. Otherwise, skips newline.
				if (Char::isChar(yaml[position++], '#'))
				{
					YAMLNode<Char> *commentNode =
						allocateNode(Sip::Comment, 0, 0, &yaml[position]);
					while (!Char::isChar(yaml[position], '\n') &&
							yaml[position] != 0)
					{
						++position;
					}
					commentNode->setValue(commentNode->value(),
						&yaml[position] - commentNode->value());
					if (node)
					{
						node->appendNode(commentNode);
					}
					else
					{
						node = commentNode;
					}
					++position;
				}

				// Add new parent.
				if (indent == indents.top())
				{
					inserting->appendNode(node);
				}
				else if (indent > indents.top())
				{
					inserting = inserting->lastChild();
					inserting->appendNode(node);
					indents.push(indent);
				}
				else
				{
					while (indent < indents.top())
					{
						inserting = inserting->parent();
						indents.pop();
					}
					if (indent != indents.top())
					{
						// TODO: Error, wrong indent level.
					}
					inserting->appendNode(node);
				}
				node = nullptr;
			}
		}
	private:
		
		static inline bool notEnd(const CharType ch)
		{
			return !(Char::isChar(ch, '\0') || Char::isChar(ch, '\n') ||
					Char::isChar(ch, '#'));
		}
		
		static inline bool isKeyChar(const CharType *ch)
		{
			return (Char::isChar(*ch, ' ') && Char::isChar(*(ch - 1), ':'));
		}
		
		static inline bool notEndKey(const CharType *ch)
		{
			return notEnd(*ch) && !isKeyChar(ch);
		}
	};

	struct YAMLDocumentUTF8 : public YAMLDocument<Unicode::CharUTF8> {};
#ifndef SIPYAML_BIG_ENDIAN
	struct YAMLDocumentUTF16LE :
		public YAMLDocument<Unicode::CharUTF16> {};
	struct YAMLDocumentUTF16BE :
		public YAMLDocument<Unicode::CharUTF16Inverse> {};
#else
	struct YAMLDocumentUTF16LE :
		public YAMLDocument<Unicode::CharUTF16Inverse> {};
	struct YAMLDocumentUTF16BE :
		public YAMLDocument<Unicode::CharUTF16> {};
#endif
}

#endif // SIPYAML__H_eTNcyHjx