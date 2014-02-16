SipYAML
==========
SipYAML is a C++ library than aims to implement a fast in-situ DOM parser
capable of reading and writing a simplified YAML specification.

Installation
------------
SipYAML is a header only library, meaning that you only need to include the
supplied header file into your application.

Design
------
Most of the design was inspired by rapidxml. As such, it has similar
limitations:

 - Node strings are *not* null terminated. Instead, you must use provided size
 functions to get the length of each string.
 - The YAML string must always be in scope. This is because node strings point
 back to the original string.
 
Tutorial
--------
### Reading ####
The following is a small, but complete example on parsing a YAML file:
// TODO

### Writing ####
The following is a small, but complete example on creating a YAML file:
// TODO
 
FAQs
----

 1. How much of the YAML specification is implemented?
 
 Only basic structures are implemented, including maps, sequences, document
 begin/end markers, directives and comments. Directives are not internally
 processed (so it doesn't matter with version of YAML you state, for
 example). Escaping, anchors, tags, casts and other various tokens are not
 implemented and may not be implemented (unless I receive enough requests).

 NOTE: As of the current version, document end markers are not implemented.
 
 2. What is in-situ?
 
 In-situ is Latin for "in place". All nodes point back to the original
 location (rather than copying strings). See the "Design" section for more
 information.
 
 3. How well is Unicode supported?
 
 The YAML specification states that UTF-8 and UTF-16 (big endian and little
 endian) must be supported and as such, there is a separate class for each of
 these. The BOM is not read by the document parsers, so your application must
 skip it, if necessary, before calling `parse()`. If you really need it, you may
 use `Sip::Unicode::Bom Sip::Unicode::checkBom(const char *)`.