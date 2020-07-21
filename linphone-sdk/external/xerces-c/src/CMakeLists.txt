############################################################################
# CMakeLists.txt
# Copyright (C) 2017  Belledonne Communications, Grenoble France
#
############################################################################
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
############################################################################

include(GNUInstallDirs)

set(DOM_HEADERS
	xercesc/dom/DOM.hpp
	xercesc/dom/DOMAttr.hpp
	xercesc/dom/DOMCDATASection.hpp
	xercesc/dom/DOMCharacterData.hpp
	xercesc/dom/DOMComment.hpp
	xercesc/dom/DOMConfiguration.hpp
	xercesc/dom/DOMDocument.hpp
	xercesc/dom/DOMDocumentFragment.hpp
	xercesc/dom/DOMDocumentRange.hpp
	xercesc/dom/DOMDocumentTraversal.hpp
	xercesc/dom/DOMDocumentType.hpp
	xercesc/dom/DOMElement.hpp
	xercesc/dom/DOMEntity.hpp
	xercesc/dom/DOMEntityReference.hpp
	xercesc/dom/DOMError.hpp
	xercesc/dom/DOMErrorHandler.hpp
	xercesc/dom/DOMException.hpp
	xercesc/dom/DOMImplementation.hpp
	xercesc/dom/DOMImplementationList.hpp
	xercesc/dom/DOMImplementationLS.hpp
	xercesc/dom/DOMImplementationRegistry.hpp
	xercesc/dom/DOMImplementationSource.hpp
	xercesc/dom/DOMLocator.hpp
	xercesc/dom/DOMLSException.hpp
	xercesc/dom/DOMLSInput.hpp
	xercesc/dom/DOMLSOutput.hpp
	xercesc/dom/DOMLSParser.hpp
	xercesc/dom/DOMLSParserFilter.hpp
	xercesc/dom/DOMLSResourceResolver.hpp
	xercesc/dom/DOMLSSerializer.hpp
	xercesc/dom/DOMLSSerializerFilter.hpp
	xercesc/dom/DOMMemoryManager.hpp
	xercesc/dom/DOMNamedNodeMap.hpp
	xercesc/dom/DOMNode.hpp
	xercesc/dom/DOMNodeFilter.hpp
	xercesc/dom/DOMNodeIterator.hpp
	xercesc/dom/DOMNodeList.hpp
	xercesc/dom/DOMNotation.hpp
	xercesc/dom/DOMProcessingInstruction.hpp
	xercesc/dom/DOMPSVITypeInfo.hpp
	xercesc/dom/DOMRange.hpp
	xercesc/dom/DOMRangeException.hpp
	xercesc/dom/DOMStringList.hpp
	xercesc/dom/DOMText.hpp
	xercesc/dom/DOMTreeWalker.hpp
	xercesc/dom/DOMTypeInfo.hpp
	xercesc/dom/DOMUserDataHandler.hpp
	xercesc/dom/DOMXPathEvaluator.hpp
	xercesc/dom/DOMXPathException.hpp
	xercesc/dom/DOMXPathExpression.hpp
	xercesc/dom/DOMXPathNamespace.hpp
	xercesc/dom/DOMXPathNSResolver.hpp
	xercesc/dom/DOMXPathResult.hpp
	xercesc/dom/StDOMNode.hpp
)

set(DOM_SOURCES
	xercesc/dom/DOMException.cpp
	xercesc/dom/DOMLSException.cpp
	xercesc/dom/DOMRangeException.cpp
	xercesc/dom/DOMXPathException.cpp
)

set(DOMIMPL_HEADERS
	xercesc/dom/impl/DOMAttrImpl.hpp
	xercesc/dom/impl/DOMAttrMapImpl.hpp
	xercesc/dom/impl/DOMAttrNSImpl.hpp
	xercesc/dom/impl/DOMCasts.hpp
	xercesc/dom/impl/DOMCDATASectionImpl.hpp
	xercesc/dom/impl/DOMCharacterDataImpl.hpp
	xercesc/dom/impl/DOMChildNode.hpp
	xercesc/dom/impl/DOMCommentImpl.hpp
	xercesc/dom/impl/DOMConfigurationImpl.hpp
	xercesc/dom/impl/DOMDeepNodeListImpl.hpp
	xercesc/dom/impl/DOMDeepNodeListPool.hpp
	xercesc/dom/impl/DOMDeepNodeListPool.c
	xercesc/dom/impl/DOMDocumentFragmentImpl.hpp
	xercesc/dom/impl/DOMDocumentImpl.hpp
	xercesc/dom/impl/DOMDocumentTypeImpl.hpp
	xercesc/dom/impl/DOMElementImpl.hpp
	xercesc/dom/impl/DOMElementNSImpl.hpp
	xercesc/dom/impl/DOMEntityImpl.hpp
	xercesc/dom/impl/DOMEntityReferenceImpl.hpp
	xercesc/dom/impl/DOMErrorImpl.hpp
	xercesc/dom/impl/DOMImplementationImpl.hpp
	xercesc/dom/impl/DOMImplementationListImpl.hpp
	xercesc/dom/impl/DOMLocatorImpl.hpp
	xercesc/dom/impl/DOMNamedNodeMapImpl.hpp
	xercesc/dom/impl/DOMNodeIDMap.hpp
	xercesc/dom/impl/DOMNodeImpl.hpp
	xercesc/dom/impl/DOMNodeIteratorImpl.hpp
	xercesc/dom/impl/DOMNodeListImpl.hpp
	xercesc/dom/impl/DOMNodeVector.hpp
	xercesc/dom/impl/DOMNormalizer.hpp
	xercesc/dom/impl/DOMNotationImpl.hpp
	xercesc/dom/impl/DOMParentNode.hpp
	xercesc/dom/impl/DOMProcessingInstructionImpl.hpp
	xercesc/dom/impl/DOMRangeImpl.hpp
	xercesc/dom/impl/DOMStringListImpl.hpp
	xercesc/dom/impl/DOMStringPool.hpp
	xercesc/dom/impl/DOMTextImpl.hpp
	xercesc/dom/impl/DOMTreeWalkerImpl.hpp
	xercesc/dom/impl/DOMTypeInfoImpl.hpp
	xercesc/dom/impl/DOMLSSerializerImpl.hpp
	xercesc/dom/impl/DOMLSInputImpl.hpp
	xercesc/dom/impl/DOMLSOutputImpl.hpp
	xercesc/dom/impl/DOMXPathExpressionImpl.hpp
	xercesc/dom/impl/DOMXPathNSResolverImpl.hpp
	xercesc/dom/impl/DOMXPathResultImpl.hpp
	xercesc/dom/impl/XSDElementNSImpl.hpp
)

set(DOMIMPL_SOURCES
	xercesc/dom/impl/DOMAttrImpl.cpp
	xercesc/dom/impl/DOMAttrMapImpl.cpp
	xercesc/dom/impl/DOMAttrNSImpl.cpp
	xercesc/dom/impl/DOMCDATASectionImpl.cpp
	xercesc/dom/impl/DOMCharacterDataImpl.cpp
	xercesc/dom/impl/DOMChildNode.cpp
	xercesc/dom/impl/DOMCommentImpl.cpp
	xercesc/dom/impl/DOMConfigurationImpl.cpp
	xercesc/dom/impl/DOMDeepNodeListImpl.cpp
	xercesc/dom/impl/DOMDocumentFragmentImpl.cpp
	xercesc/dom/impl/DOMDocumentImpl.cpp
	xercesc/dom/impl/DOMDocumentTypeImpl.cpp
	xercesc/dom/impl/DOMElementImpl.cpp
	xercesc/dom/impl/DOMElementNSImpl.cpp
	xercesc/dom/impl/DOMEntityImpl.cpp
	xercesc/dom/impl/DOMEntityReferenceImpl.cpp
	xercesc/dom/impl/DOMErrorImpl.cpp
	xercesc/dom/impl/DOMImplementationImpl.cpp
	xercesc/dom/impl/DOMImplementationListImpl.cpp
	xercesc/dom/impl/DOMImplementationRegistry.cpp
	xercesc/dom/impl/DOMLocatorImpl.cpp
	xercesc/dom/impl/DOMNamedNodeMapImpl.cpp
	xercesc/dom/impl/DOMNodeIDMap.cpp
	xercesc/dom/impl/DOMNodeImpl.cpp
	xercesc/dom/impl/DOMNodeIteratorImpl.cpp
	xercesc/dom/impl/DOMNodeListImpl.cpp
	xercesc/dom/impl/DOMNodeVector.cpp
	xercesc/dom/impl/DOMNormalizer.cpp
	xercesc/dom/impl/DOMNotationImpl.cpp
	xercesc/dom/impl/DOMParentNode.cpp
	xercesc/dom/impl/DOMProcessingInstructionImpl.cpp
	xercesc/dom/impl/DOMRangeImpl.cpp
	xercesc/dom/impl/DOMStringListImpl.cpp
	xercesc/dom/impl/DOMStringPool.cpp
	xercesc/dom/impl/DOMTextImpl.cpp
	xercesc/dom/impl/DOMTreeWalkerImpl.cpp
	xercesc/dom/impl/DOMTypeInfoImpl.cpp
	xercesc/dom/impl/DOMLSSerializerImpl.cpp
	xercesc/dom/impl/DOMLSInputImpl.cpp
	xercesc/dom/impl/DOMLSOutputImpl.cpp
	xercesc/dom/impl/DOMXPathExpressionImpl.cpp
	xercesc/dom/impl/DOMXPathNSResolverImpl.cpp
	xercesc/dom/impl/DOMXPathResultImpl.cpp
	xercesc/dom/impl/XSDElementNSImpl.cpp
)

set(FRAMEWORK_HEADERS
	xercesc/framework/BinOutputStream.hpp
	xercesc/framework/LocalFileFormatTarget.hpp
	xercesc/framework/LocalFileInputSource.hpp
	xercesc/framework/MemBufFormatTarget.hpp
	xercesc/framework/MemBufInputSource.hpp
	xercesc/framework/MemoryManager.hpp
	xercesc/framework/StdInInputSource.hpp
	xercesc/framework/StdOutFormatTarget.hpp
	xercesc/framework/URLInputSource.hpp
	xercesc/framework/ValidationContext.hpp
	xercesc/framework/Wrapper4DOMLSInput.hpp
	xercesc/framework/Wrapper4InputSource.hpp
	xercesc/framework/XMLAttDef.hpp
	xercesc/framework/XMLAttDefList.hpp
	xercesc/framework/XMLAttr.hpp
	xercesc/framework/XMLBuffer.hpp
	xercesc/framework/XMLBufferMgr.hpp
	xercesc/framework/XMLContentModel.hpp
	xercesc/framework/XMLDocumentHandler.hpp
	xercesc/framework/XMLDTDDescription.hpp
	xercesc/framework/XMLElementDecl.hpp
	xercesc/framework/XMLEntityDecl.hpp
	xercesc/framework/XMLEntityHandler.hpp
	xercesc/framework/XMLErrorCodes.hpp
	xercesc/framework/XMLErrorReporter.hpp
	xercesc/framework/XMLFormatter.hpp
	xercesc/framework/XMLGrammarDescription.hpp
	xercesc/framework/XMLGrammarPool.hpp
	xercesc/framework/XMLGrammarPoolImpl.hpp
	xercesc/framework/XMLNotationDecl.hpp
	xercesc/framework/XMLPScanToken.hpp
	xercesc/framework/XMLRecognizer.hpp
	xercesc/framework/XMLRefInfo.hpp
	xercesc/framework/XMLSchemaDescription.hpp
	xercesc/framework/XMLValidator.hpp
	xercesc/framework/XMLValidityCodes.hpp
)

set(FRAMEWORK_PSVI_HEADERS
	xercesc/framework/psvi/PSVIAttribute.hpp
	xercesc/framework/psvi/PSVIAttributeList.hpp
	xercesc/framework/psvi/PSVIElement.hpp
	xercesc/framework/psvi/PSVIHandler.hpp
	xercesc/framework/psvi/PSVIItem.hpp
	xercesc/framework/psvi/XSAnnotation.hpp
	xercesc/framework/psvi/XSAttributeDeclaration.hpp
	xercesc/framework/psvi/XSAttributeGroupDefinition.hpp
	xercesc/framework/psvi/XSAttributeUse.hpp
	xercesc/framework/psvi/XSComplexTypeDefinition.hpp
	xercesc/framework/psvi/XSConstants.hpp
	xercesc/framework/psvi/XSElementDeclaration.hpp
	xercesc/framework/psvi/XSFacet.hpp
	xercesc/framework/psvi/XSIDCDefinition.hpp
	xercesc/framework/psvi/XSModel.hpp
	xercesc/framework/psvi/XSModelGroup.hpp
	xercesc/framework/psvi/XSModelGroupDefinition.hpp
	xercesc/framework/psvi/XSMultiValueFacet.hpp
	xercesc/framework/psvi/XSNamedMap.hpp
	xercesc/framework/psvi/XSNamedMap.c
	xercesc/framework/psvi/XSNamespaceItem.hpp
	xercesc/framework/psvi/XSNotationDeclaration.hpp
	xercesc/framework/psvi/XSObject.hpp
	xercesc/framework/psvi/XSParticle.hpp
	xercesc/framework/psvi/XSSimpleTypeDefinition.hpp
	xercesc/framework/psvi/XSTypeDefinition.hpp
	xercesc/framework/psvi/XSValue.hpp
	xercesc/framework/psvi/XSWildcard.hpp
)

set(FRAMEWORK_SOURCES
	xercesc/framework/BinOutputStream.cpp
	xercesc/framework/LocalFileFormatTarget.cpp
	xercesc/framework/LocalFileInputSource.cpp
	xercesc/framework/MemBufFormatTarget.cpp
	xercesc/framework/MemBufInputSource.cpp
	xercesc/framework/psvi/PSVIAttribute.cpp
	xercesc/framework/psvi/PSVIAttributeList.cpp
	xercesc/framework/psvi/PSVIElement.cpp
	xercesc/framework/psvi/PSVIItem.cpp
	xercesc/framework/psvi/XSAnnotation.cpp
	xercesc/framework/psvi/XSAttributeDeclaration.cpp
	xercesc/framework/psvi/XSAttributeGroupDefinition.cpp
	xercesc/framework/psvi/XSAttributeUse.cpp
	xercesc/framework/psvi/XSComplexTypeDefinition.cpp
	xercesc/framework/psvi/XSElementDeclaration.cpp
	xercesc/framework/psvi/XSFacet.cpp
	xercesc/framework/psvi/XSIDCDefinition.cpp
	xercesc/framework/psvi/XSModel.cpp
	xercesc/framework/psvi/XSModelGroup.cpp
	xercesc/framework/psvi/XSModelGroupDefinition.cpp
	xercesc/framework/psvi/XSMultiValueFacet.cpp
	xercesc/framework/psvi/XSNamespaceItem.cpp
	xercesc/framework/psvi/XSNotationDeclaration.cpp
	xercesc/framework/psvi/XSObject.cpp
	xercesc/framework/psvi/XSParticle.cpp
	xercesc/framework/psvi/XSSimpleTypeDefinition.cpp
	xercesc/framework/psvi/XSTypeDefinition.cpp
	xercesc/framework/psvi/XSValue.cpp
	xercesc/framework/psvi/XSWildcard.cpp
	xercesc/framework/StdInInputSource.cpp
	xercesc/framework/StdOutFormatTarget.cpp
	xercesc/framework/URLInputSource.cpp
	xercesc/framework/Wrapper4DOMLSInput.cpp
	xercesc/framework/Wrapper4InputSource.cpp
	xercesc/framework/XMLAttDef.cpp
	xercesc/framework/XMLAttDefList.cpp
	xercesc/framework/XMLAttr.cpp
	xercesc/framework/XMLBuffer.cpp
	xercesc/framework/XMLBufferMgr.cpp
	xercesc/framework/XMLContentModel.cpp
	xercesc/framework/XMLDTDDescription.cpp
	xercesc/framework/XMLElementDecl.cpp
	xercesc/framework/XMLEntityDecl.cpp
	xercesc/framework/XMLFormatter.cpp
	xercesc/framework/XMLGrammarDescription.cpp
	xercesc/framework/XMLGrammarPoolImpl.cpp
	xercesc/framework/XMLNotationDecl.cpp
	xercesc/framework/XMLRecognizer.cpp
	xercesc/framework/XMLRefInfo.cpp
	xercesc/framework/XMLSchemaDescription.cpp
	xercesc/framework/XMLValidator.cpp
)

set(INTERNAL_HEADERS
	xercesc/internal/BinFileOutputStream.hpp
	xercesc/internal/BinMemOutputStream.hpp
	xercesc/internal/CharTypeTables.hpp
	xercesc/internal/DGXMLScanner.hpp
	xercesc/internal/ElemStack.hpp
	xercesc/internal/EndOfEntityException.hpp
	xercesc/internal/IANAEncodings.hpp
	xercesc/internal/IGXMLScanner.hpp
	xercesc/internal/MemoryManagerImpl.hpp
	xercesc/internal/ReaderMgr.hpp
	xercesc/internal/SGXMLScanner.hpp
	xercesc/internal/ValidationContextImpl.hpp
	xercesc/internal/VecAttributesImpl.hpp
	xercesc/internal/VecAttrListImpl.hpp
	xercesc/internal/WFXMLScanner.hpp
	xercesc/internal/XMLInternalErrorHandler.hpp
	xercesc/internal/XMLReader.hpp
	xercesc/internal/XMLScanner.hpp
	xercesc/internal/XMLScannerResolver.hpp
	xercesc/internal/XProtoType.hpp
	xercesc/internal/XSAXMLScanner.hpp
	xercesc/internal/XSerializable.hpp
	xercesc/internal/XSerializationException.hpp
	xercesc/internal/XSerializeEngine.hpp
	xercesc/internal/XSObjectFactory.hpp
	xercesc/internal/XTemplateSerializer.hpp
)

set(INTERNAL_SOURCES
	xercesc/internal/BinFileOutputStream.cpp
	xercesc/internal/BinMemOutputStream.cpp
	xercesc/internal/DGXMLScanner.cpp
	xercesc/internal/ElemStack.cpp
	xercesc/internal/IGXMLScanner.cpp
	xercesc/internal/IGXMLScanner2.cpp
	xercesc/internal/MemoryManagerImpl.cpp
	xercesc/internal/ReaderMgr.cpp
	xercesc/internal/SGXMLScanner.cpp
	xercesc/internal/ValidationContextImpl.cpp
	xercesc/internal/VecAttributesImpl.cpp
	xercesc/internal/VecAttrListImpl.cpp
	xercesc/internal/WFXMLScanner.cpp
	xercesc/internal/XMLReader.cpp
	xercesc/internal/XMLScanner.cpp
	xercesc/internal/XMLScannerResolver.cpp
	xercesc/internal/XProtoType.cpp
	xercesc/internal/XSAXMLScanner.cpp
	xercesc/internal/XSerializeEngine.cpp
	xercesc/internal/XSObjectFactory.cpp
	xercesc/internal/XTemplateSerializer.cpp
)

set(PARSERS_HEADERS
	xercesc/parsers/AbstractDOMParser.hpp
	xercesc/parsers/DOMLSParserImpl.hpp
	xercesc/parsers/SAX2XMLFilterImpl.hpp
	xercesc/parsers/SAX2XMLReaderImpl.hpp
	xercesc/parsers/SAXParser.hpp
	xercesc/parsers/XercesDOMParser.hpp
)

set(PARSERS_SOURCES
	xercesc/parsers/AbstractDOMParser.cpp
	xercesc/parsers/DOMLSParserImpl.cpp
	xercesc/parsers/SAX2XMLFilterImpl.cpp
	xercesc/parsers/SAX2XMLReaderImpl.cpp
	xercesc/parsers/SAXParser.cpp
	xercesc/parsers/XercesDOMParser.cpp
)

set(SAX_HEADERS
	xercesc/sax/AttributeList.hpp
	xercesc/sax/DocumentHandler.hpp
	xercesc/sax/DTDHandler.hpp
	xercesc/sax/EntityResolver.hpp
	xercesc/sax/ErrorHandler.hpp
	xercesc/sax/HandlerBase.hpp
	xercesc/sax/InputSource.hpp
	xercesc/sax/Locator.hpp
	xercesc/sax/Parser.hpp
	xercesc/sax/SAXException.hpp
	xercesc/sax/SAXParseException.hpp
)

set(SAX_SOURCES
	xercesc/sax/Dummy.cpp
	xercesc/sax/InputSource.cpp
	xercesc/sax/SAXException.cpp
	xercesc/sax/SAXParseException.cpp
)

set(SAX2_HEADERS
	xercesc/sax2/Attributes.hpp
	xercesc/sax2/ContentHandler.hpp
	xercesc/sax2/DeclHandler.hpp
	xercesc/sax2/DefaultHandler.hpp
	xercesc/sax2/LexicalHandler.hpp
	xercesc/sax2/SAX2XMLFilter.hpp
	xercesc/sax2/SAX2XMLReader.hpp
	xercesc/sax2/XMLReaderFactory.hpp
)

set(SAX2_SOURCES
	xercesc/sax2/sax2Dummy.cpp
)

set(UTIL_HEADERS
	xercesc/util/ArrayIndexOutOfBoundsException.hpp
	xercesc/util/Base64.hpp
	xercesc/util/BaseRefVectorOf.hpp
	xercesc/util/BaseRefVectorOf.c
	xercesc/util/BinFileInputStream.hpp
	xercesc/util/BinInputStream.hpp
	xercesc/util/BinMemInputStream.hpp
	xercesc/util/BitOps.hpp
	xercesc/util/BitSet.hpp
	xercesc/util/CountedPointer.hpp
	xercesc/util/CountedPointer.c
	xercesc/util/DefaultPanicHandler.hpp
	xercesc/util/EmptyStackException.hpp
	xercesc/util/EncodingValidator.hpp
	xercesc/util/FlagJanitor.hpp
	xercesc/util/FlagJanitor.c
	xercesc/util/Hashers.hpp
	xercesc/util/Hash2KeysSetOf.hpp
	xercesc/util/Hash2KeysSetOf.c
	xercesc/util/HexBin.hpp
	xercesc/util/IllegalArgumentException.hpp
	xercesc/util/InvalidCastException.hpp
	xercesc/util/IOException.hpp
	xercesc/util/Janitor.hpp
	xercesc/util/Janitor.c
	xercesc/util/KeyRefPair.hpp
	xercesc/util/KeyRefPair.c
	xercesc/util/KeyValuePair.hpp
	xercesc/util/KeyValuePair.c
	xercesc/util/KVStringPair.hpp
	xercesc/util/Mutexes.hpp
	xercesc/util/LogicalPath.c
	xercesc/util/NameIdPool.hpp
	xercesc/util/NameIdPool.c
	xercesc/util/NoSuchElementException.hpp
	xercesc/util/NullPointerException.hpp
	xercesc/util/NumberFormatException.hpp
	xercesc/util/OutOfMemoryException.hpp
	xercesc/util/PanicHandler.hpp
	xercesc/util/ParseException.hpp
	xercesc/util/PlatformUtils.hpp
	xercesc/util/PSVIUni.hpp
	xercesc/util/QName.hpp
	xercesc/util/RefArrayOf.hpp
	xercesc/util/RefArrayOf.c
	xercesc/util/RefArrayVectorOf.hpp
	xercesc/util/RefArrayVectorOf.c
	xercesc/util/RefHash2KeysTableOf.hpp
	xercesc/util/RefHash2KeysTableOf.c
	xercesc/util/RefHash3KeysIdPool.hpp
	xercesc/util/RefHash3KeysIdPool.c
	xercesc/util/RefHashTableOf.hpp
	xercesc/util/RefHashTableOf.c
	xercesc/util/RefStackOf.hpp
	xercesc/util/RefStackOf.c
	xercesc/util/RefVectorOf.hpp
	xercesc/util/RefVectorOf.c
	xercesc/util/RuntimeException.hpp
	xercesc/util/SchemaDateTimeException.hpp
	xercesc/util/SecurityManager.hpp
	xercesc/util/StringPool.hpp
	xercesc/util/SynchronizedStringPool.hpp
	xercesc/util/TranscodingException.hpp
	xercesc/util/TransENameMap.hpp
	xercesc/util/TransENameMap.c
	xercesc/util/TransService.hpp
	xercesc/util/UnexpectedEOFException.hpp
	xercesc/util/UnsupportedEncodingException.hpp
	xercesc/util/UTFDataFormatException.hpp
	xercesc/util/ValueArrayOf.hpp
	xercesc/util/ValueArrayOf.c
	xercesc/util/ValueHashTableOf.hpp
	xercesc/util/ValueHashTableOf.c
	xercesc/util/ValueStackOf.hpp
	xercesc/util/ValueStackOf.c
	xercesc/util/ValueVectorOf.hpp
	xercesc/util/ValueVectorOf.c
	xercesc/util/XercesDefs.hpp
	xercesc/util/XercesVersion.hpp
	xercesc/util/XMemory.hpp
	xercesc/util/XML256TableTranscoder.hpp
	xercesc/util/XML88591Transcoder.hpp
	xercesc/util/XMLAbstractDoubleFloat.hpp
	xercesc/util/XMLASCIITranscoder.hpp
	xercesc/util/XMLBigDecimal.hpp
	xercesc/util/XMLBigInteger.hpp
	xercesc/util/XMLChar.hpp
	xercesc/util/XMLChTranscoder.hpp
	xercesc/util/XMLDateTime.hpp
	xercesc/util/XMLDOMMsg.hpp
	xercesc/util/XMLDouble.hpp
	xercesc/util/XMLEBCDICTranscoder.hpp
	xercesc/util/XMLEntityResolver.hpp
	xercesc/util/XMLEnumerator.hpp
	xercesc/util/XMLException.hpp
	xercesc/util/XMLExceptMsgs.hpp
	xercesc/util/XMLFileMgr.hpp
	xercesc/util/XMLFloat.hpp
	xercesc/util/XMLIBM1047Transcoder.hpp
	xercesc/util/XMLIBM1140Transcoder.hpp
	xercesc/util/XMLInitializer.hpp
	xercesc/util/XMLInteger.hpp
	xercesc/util/XMLMutexMgr.hpp
	xercesc/util/XMLMsgLoader.hpp
	xercesc/util/XMLNetAccessor.hpp
	xercesc/util/XMLNumber.hpp
	xercesc/util/XMLResourceIdentifier.hpp
	xercesc/util/XMLString.hpp
	xercesc/util/XMLStringTokenizer.hpp
	xercesc/util/XMLUCS4Transcoder.hpp
	xercesc/util/XMLUni.hpp
	xercesc/util/XMLUniDefs.hpp
	xercesc/util/XMLUri.hpp
	xercesc/util/XMLURL.hpp
	xercesc/util/XMLUTF16Transcoder.hpp
	xercesc/util/XMLUTF8Transcoder.hpp
	xercesc/util/XMLWin1252Transcoder.hpp
)

set(UTIL_REGX_HEADERS
	xercesc/util/regx/ASCIIRangeFactory.hpp
	xercesc/util/regx/BlockRangeFactory.hpp
	xercesc/util/regx/BMPattern.hpp
	xercesc/util/regx/CharToken.hpp
	xercesc/util/regx/ClosureToken.hpp
	xercesc/util/regx/ConcatToken.hpp
	xercesc/util/regx/Match.hpp
	xercesc/util/regx/Op.hpp
	xercesc/util/regx/OpFactory.hpp
	xercesc/util/regx/ParenToken.hpp
	xercesc/util/regx/ParserForXMLSchema.hpp
	xercesc/util/regx/RangeFactory.hpp
	xercesc/util/regx/RangeToken.hpp
	xercesc/util/regx/RangeTokenMap.hpp
	xercesc/util/regx/RegularExpression.hpp
	xercesc/util/regx/RegxDefs.hpp
	xercesc/util/regx/RegxParser.hpp
	xercesc/util/regx/RegxUtil.hpp
	xercesc/util/regx/StringToken.hpp
	xercesc/util/regx/Token.hpp
	xercesc/util/regx/TokenFactory.hpp
	xercesc/util/regx/TokenInc.hpp
	xercesc/util/regx/UniCharTable.hpp
	xercesc/util/regx/UnicodeRangeFactory.hpp
	xercesc/util/regx/UnionToken.hpp
	xercesc/util/regx/XMLRangeFactory.hpp
	xercesc/util/regx/XMLUniCharacter.hpp
)

set(UTIL_SOURCES
	xercesc/util/Base64.cpp
	xercesc/util/BinFileInputStream.cpp
	xercesc/util/BinInputStream.cpp
	xercesc/util/BinMemInputStream.cpp
	xercesc/util/BitSet.cpp
	xercesc/util/DefaultPanicHandler.cpp
	xercesc/util/EncodingValidator.cpp
	xercesc/util/HeaderDummy.cpp
	xercesc/util/HexBin.cpp
	xercesc/util/KVStringPair.cpp
	xercesc/util/Mutexes.cpp
	xercesc/util/PanicHandler.cpp
	xercesc/util/PlatformUtils.cpp
	xercesc/util/PSVIUni.cpp
	xercesc/util/QName.cpp
	xercesc/util/regx/ASCIIRangeFactory.cpp
	xercesc/util/regx/BlockRangeFactory.cpp
	xercesc/util/regx/BMPattern.cpp
	xercesc/util/regx/CharToken.cpp
	xercesc/util/regx/ClosureToken.cpp
	xercesc/util/regx/ConcatToken.cpp
	xercesc/util/regx/Match.cpp
	xercesc/util/regx/Op.cpp
	xercesc/util/regx/OpFactory.cpp
	xercesc/util/regx/ParenToken.cpp
	xercesc/util/regx/ParserForXMLSchema.cpp
	xercesc/util/regx/RangeFactory.cpp
	xercesc/util/regx/RangeToken.cpp
	xercesc/util/regx/RangeTokenMap.cpp
	xercesc/util/regx/RegularExpression.cpp
	xercesc/util/regx/RegxParser.cpp
	xercesc/util/regx/RegxUtil.cpp
	xercesc/util/regx/StringToken.cpp
	xercesc/util/regx/Token.cpp
	xercesc/util/regx/TokenFactory.cpp
	xercesc/util/regx/UnicodeRangeFactory.cpp
	xercesc/util/regx/UnionToken.cpp
	xercesc/util/regx/XMLRangeFactory.cpp
	xercesc/util/regx/XMLUniCharacter.cpp
	xercesc/util/StringPool.cpp
	xercesc/util/SynchronizedStringPool.cpp
	xercesc/util/TransService.cpp
	xercesc/util/XMemory.cpp
	xercesc/util/XML256TableTranscoder.cpp
	xercesc/util/XML88591Transcoder.cpp
	xercesc/util/XMLAbstractDoubleFloat.cpp
	xercesc/util/XMLASCIITranscoder.cpp
	xercesc/util/XMLBigDecimal.cpp
	xercesc/util/XMLBigInteger.cpp
	xercesc/util/XMLChar.cpp
	xercesc/util/XMLChTranscoder.cpp
	xercesc/util/XMLDateTime.cpp
	xercesc/util/XMLDouble.cpp
	xercesc/util/XMLEBCDICTranscoder.cpp
	xercesc/util/XMLException.cpp
	xercesc/util/XMLFloat.cpp
	xercesc/util/XMLIBM1047Transcoder.cpp
	xercesc/util/XMLIBM1140Transcoder.cpp
	xercesc/util/XMLInitializer.cpp
	xercesc/util/XMLMsgLoader.cpp
	xercesc/util/XMLNumber.cpp
	xercesc/util/XMLString.cpp
	xercesc/util/XMLStringTokenizer.cpp
	xercesc/util/XMLUCS4Transcoder.cpp
	xercesc/util/XMLUni.cpp
	xercesc/util/XMLUri.cpp
	xercesc/util/XMLURL.cpp
	xercesc/util/XMLUTF16Transcoder.cpp
	xercesc/util/XMLUTF8Transcoder.cpp
	xercesc/util/XMLWin1252Transcoder.cpp
)

set(VALIDATORS_COMMON_HEADERS
	xercesc/validators/common/AllContentModel.hpp
	xercesc/validators/common/CMAny.hpp
	xercesc/validators/common/CMBinaryOp.hpp
	xercesc/validators/common/CMLeaf.hpp
	xercesc/validators/common/CMRepeatingLeaf.hpp
	xercesc/validators/common/CMNode.hpp
	xercesc/validators/common/CMStateSet.hpp
	xercesc/validators/common/CMUnaryOp.hpp
	xercesc/validators/common/ContentLeafNameTypeVector.hpp
	xercesc/validators/common/ContentSpecNode.hpp
	xercesc/validators/common/DFAContentModel.hpp
	xercesc/validators/common/Grammar.hpp
	xercesc/validators/common/GrammarResolver.hpp
	xercesc/validators/common/MixedContentModel.hpp
	xercesc/validators/common/SimpleContentModel.hpp
)

set(VALIDATORS_DATATYPE_HEADERS
	xercesc/validators/datatype/AbstractNumericFacetValidator.hpp
	xercesc/validators/datatype/AbstractNumericValidator.hpp
	xercesc/validators/datatype/AbstractStringValidator.hpp
	xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.hpp
	xercesc/validators/datatype/AnyURIDatatypeValidator.hpp
	xercesc/validators/datatype/Base64BinaryDatatypeValidator.hpp
	xercesc/validators/datatype/BooleanDatatypeValidator.hpp
	xercesc/validators/datatype/DatatypeValidator.hpp
	xercesc/validators/datatype/DatatypeValidatorFactory.hpp
	xercesc/validators/datatype/DateDatatypeValidator.hpp
	xercesc/validators/datatype/DateTimeDatatypeValidator.hpp
	xercesc/validators/datatype/DateTimeValidator.hpp
	xercesc/validators/datatype/DayDatatypeValidator.hpp
	xercesc/validators/datatype/DecimalDatatypeValidator.hpp
	xercesc/validators/datatype/DoubleDatatypeValidator.hpp
	xercesc/validators/datatype/DurationDatatypeValidator.hpp
	xercesc/validators/datatype/ENTITYDatatypeValidator.hpp
	xercesc/validators/datatype/FloatDatatypeValidator.hpp
	xercesc/validators/datatype/HexBinaryDatatypeValidator.hpp
	xercesc/validators/datatype/IDDatatypeValidator.hpp
	xercesc/validators/datatype/IDREFDatatypeValidator.hpp
	xercesc/validators/datatype/InvalidDatatypeFacetException.hpp
	xercesc/validators/datatype/InvalidDatatypeValueException.hpp
	xercesc/validators/datatype/ListDatatypeValidator.hpp
	xercesc/validators/datatype/MonthDatatypeValidator.hpp
	xercesc/validators/datatype/MonthDayDatatypeValidator.hpp
	xercesc/validators/datatype/NameDatatypeValidator.hpp
	xercesc/validators/datatype/NCNameDatatypeValidator.hpp
	xercesc/validators/datatype/NOTATIONDatatypeValidator.hpp
	xercesc/validators/datatype/QNameDatatypeValidator.hpp
	xercesc/validators/datatype/StringDatatypeValidator.hpp
	xercesc/validators/datatype/TimeDatatypeValidator.hpp
	xercesc/validators/datatype/UnionDatatypeValidator.hpp
	xercesc/validators/datatype/XMLCanRepGroup.hpp
	xercesc/validators/datatype/YearDatatypeValidator.hpp
	xercesc/validators/datatype/YearMonthDatatypeValidator.hpp
)

set(VALIDATORS_DTD_HEADERS
	xercesc/validators/DTD/DocTypeHandler.hpp
	xercesc/validators/DTD/DTDAttDef.hpp
	xercesc/validators/DTD/DTDAttDefList.hpp
	xercesc/validators/DTD/DTDElementDecl.hpp
	xercesc/validators/DTD/DTDEntityDecl.hpp
	xercesc/validators/DTD/DTDGrammar.hpp
	xercesc/validators/DTD/DTDScanner.hpp
	xercesc/validators/DTD/DTDValidator.hpp
	xercesc/validators/DTD/XMLDTDDescriptionImpl.hpp
)

set(VALIDATORS_SCHEMA_HEADERS
	xercesc/validators/schema/ComplexTypeInfo.hpp
	xercesc/validators/schema/GeneralAttributeCheck.hpp
	xercesc/validators/schema/identity/FieldActivator.hpp
	xercesc/validators/schema/identity/FieldValueMap.hpp
	xercesc/validators/schema/identity/IC_Field.hpp
	xercesc/validators/schema/identity/IC_Key.hpp
	xercesc/validators/schema/identity/IC_KeyRef.hpp
	xercesc/validators/schema/identity/IC_Selector.hpp
	xercesc/validators/schema/identity/IC_Unique.hpp
	xercesc/validators/schema/identity/IdentityConstraint.hpp
	xercesc/validators/schema/identity/IdentityConstraintHandler.hpp
	xercesc/validators/schema/identity/ValueStore.hpp
	xercesc/validators/schema/identity/ValueStoreCache.hpp
	xercesc/validators/schema/identity/XercesXPath.hpp
	xercesc/validators/schema/identity/XPathException.hpp
	xercesc/validators/schema/identity/XPathMatcher.hpp
	xercesc/validators/schema/identity/XPathMatcherStack.hpp
	xercesc/validators/schema/identity/XPathSymbols.hpp
	xercesc/validators/schema/NamespaceScope.hpp
	xercesc/validators/schema/PSVIDefs.hpp
	xercesc/validators/schema/SchemaAttDef.hpp
	xercesc/validators/schema/SchemaAttDefList.hpp
	xercesc/validators/schema/SchemaElementDecl.hpp
	xercesc/validators/schema/SchemaGrammar.hpp
	xercesc/validators/schema/SchemaInfo.hpp
	xercesc/validators/schema/SchemaSymbols.hpp
	xercesc/validators/schema/SchemaValidator.hpp
	xercesc/validators/schema/SubstitutionGroupComparator.hpp
	xercesc/validators/schema/TraverseSchema.hpp
	xercesc/validators/schema/XercesAttGroupInfo.hpp
	xercesc/validators/schema/XercesElementWildcard.hpp
	xercesc/validators/schema/XercesGroupInfo.hpp
	xercesc/validators/schema/XMLSchemaDescriptionImpl.hpp
	xercesc/validators/schema/XSDDOMParser.hpp
	xercesc/validators/schema/XSDErrorReporter.hpp
	xercesc/validators/schema/XSDLocator.hpp
	xercesc/validators/schema/XUtil.hpp
)

set(VALIDATORS_SOURCES
	xercesc/validators/common/AllContentModel.cpp
	xercesc/validators/common/CMAny.cpp
	xercesc/validators/common/CMBinaryOp.cpp
	xercesc/validators/common/CMUnaryOp.cpp
	xercesc/validators/common/ContentLeafNameTypeVector.cpp
	xercesc/validators/common/ContentSpecNode.cpp
	xercesc/validators/common/DFAContentModel.cpp
	xercesc/validators/common/Grammar.cpp
	xercesc/validators/common/GrammarResolver.cpp
	xercesc/validators/common/MixedContentModel.cpp
	xercesc/validators/common/SimpleContentModel.cpp
	xercesc/validators/datatype/AbstractNumericFacetValidator.cpp
	xercesc/validators/datatype/AbstractNumericValidator.cpp
	xercesc/validators/datatype/AbstractStringValidator.cpp
	xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.cpp
	xercesc/validators/datatype/AnyURIDatatypeValidator.cpp
	xercesc/validators/datatype/Base64BinaryDatatypeValidator.cpp
	xercesc/validators/datatype/BooleanDatatypeValidator.cpp
	xercesc/validators/datatype/DatatypeValidator.cpp
	xercesc/validators/datatype/DatatypeValidatorFactory.cpp
	xercesc/validators/datatype/DateDatatypeValidator.cpp
	xercesc/validators/datatype/DateTimeDatatypeValidator.cpp
	xercesc/validators/datatype/DateTimeValidator.cpp
	xercesc/validators/datatype/DayDatatypeValidator.cpp
	xercesc/validators/datatype/DecimalDatatypeValidator.cpp
	xercesc/validators/datatype/DoubleDatatypeValidator.cpp
	xercesc/validators/datatype/DurationDatatypeValidator.cpp
	xercesc/validators/datatype/ENTITYDatatypeValidator.cpp
	xercesc/validators/datatype/FloatDatatypeValidator.cpp
	xercesc/validators/datatype/HexBinaryDatatypeValidator.cpp
	xercesc/validators/datatype/IDDatatypeValidator.cpp
	xercesc/validators/datatype/IDREFDatatypeValidator.cpp
	xercesc/validators/datatype/ListDatatypeValidator.cpp
	xercesc/validators/datatype/MonthDatatypeValidator.cpp
	xercesc/validators/datatype/MonthDayDatatypeValidator.cpp
	xercesc/validators/datatype/NameDatatypeValidator.cpp
	xercesc/validators/datatype/NCNameDatatypeValidator.cpp
	xercesc/validators/datatype/NOTATIONDatatypeValidator.cpp
	xercesc/validators/datatype/QNameDatatypeValidator.cpp
	xercesc/validators/datatype/StringDatatypeValidator.cpp
	xercesc/validators/datatype/TimeDatatypeValidator.cpp
	xercesc/validators/datatype/UnionDatatypeValidator.cpp
	xercesc/validators/datatype/XMLCanRepGroup.cpp
	xercesc/validators/datatype/YearDatatypeValidator.cpp
	xercesc/validators/datatype/YearMonthDatatypeValidator.cpp
	xercesc/validators/DTD/DTDAttDef.cpp
	xercesc/validators/DTD/DTDAttDefList.cpp
	xercesc/validators/DTD/DTDElementDecl.cpp
	xercesc/validators/DTD/DTDEntityDecl.cpp
	xercesc/validators/DTD/DTDGrammar.cpp
	xercesc/validators/DTD/DTDScanner.cpp
	xercesc/validators/DTD/DTDValidator.cpp
	xercesc/validators/DTD/XMLDTDDescriptionImpl.cpp
	xercesc/validators/schema/ComplexTypeInfo.cpp
	xercesc/validators/schema/GeneralAttributeCheck.cpp
	xercesc/validators/schema/identity/FieldActivator.cpp
	xercesc/validators/schema/identity/FieldValueMap.cpp
	xercesc/validators/schema/identity/IC_Field.cpp
	xercesc/validators/schema/identity/IC_Key.cpp
	xercesc/validators/schema/identity/IC_KeyRef.cpp
	xercesc/validators/schema/identity/IC_Selector.cpp
	xercesc/validators/schema/identity/IC_Unique.cpp
	xercesc/validators/schema/identity/IdentityConstraint.cpp
	xercesc/validators/schema/identity/IdentityConstraintHandler.cpp
	xercesc/validators/schema/identity/ValueStore.cpp
	xercesc/validators/schema/identity/ValueStoreCache.cpp
	xercesc/validators/schema/identity/XercesXPath.cpp
	xercesc/validators/schema/identity/XPathMatcher.cpp
	xercesc/validators/schema/identity/XPathMatcherStack.cpp
	xercesc/validators/schema/identity/XPathSymbols.cpp
	xercesc/validators/schema/NamespaceScope.cpp
	xercesc/validators/schema/SchemaAttDef.cpp
	xercesc/validators/schema/SchemaAttDefList.cpp
	xercesc/validators/schema/SchemaElementDecl.cpp
	xercesc/validators/schema/SchemaGrammar.cpp
	xercesc/validators/schema/SchemaInfo.cpp
	xercesc/validators/schema/SchemaSymbols.cpp
	xercesc/validators/schema/SchemaValidator.cpp
	xercesc/validators/schema/SubstitutionGroupComparator.cpp
	xercesc/validators/schema/TraverseSchema.cpp
	xercesc/validators/schema/XercesAttGroupInfo.cpp
	xercesc/validators/schema/XercesElementWildcard.cpp
	xercesc/validators/schema/XercesGroupInfo.cpp
	xercesc/validators/schema/XMLSchemaDescriptionImpl.cpp
	xercesc/validators/schema/XSDDOMParser.cpp
	xercesc/validators/schema/XSDErrorReporter.cpp
	xercesc/validators/schema/XSDLocator.cpp
	xercesc/validators/schema/XUtil.cpp
)

set(XINCLUDE_HEADERS
	xercesc/xinclude/XIncludeDOMDocumentProcessor.hpp
	xercesc/xinclude/XIncludeLocation.hpp
	xercesc/xinclude/XIncludeUtils.hpp
)

set(XINCLUDE_SOURCES
	xercesc/xinclude/XIncludeDOMDocumentProcessor.cpp
	xercesc/xinclude/XIncludeLocation.cpp
	xercesc/xinclude/XIncludeUtils.cpp
)

set(ICU_HEADERS
	xercesc/util/Transcoders/ICU/ICUTransService.hpp
)

set(ICU_SOURCES
	xercesc/util/Transcoders/ICU/ICUTransService.cpp
)

set(GNUICONV_HEADERS
	xercesc/util/Transcoders/IconvGNU/IconvGNUTransService.hpp
)

set(GNUICONV_SOURCES
	xercesc/util/Transcoders/IconvGNU/IconvGNUTransService.cpp
)

set(ICONV_HEADERS
	xercesc/util/Transcoders/Iconv/IconvTransService.hpp
)

set(ICONV_SOURCES
	xercesc/util/Transcoders/Iconv/IconvTransService.cpp
)

set(MACOSUNICODECONVERTER_HEADERS
	xercesc/util/Transcoders/MacOSUnicodeConverter/MacOSUnicodeConverter.hpp
)

set(MACOSUNICODECONVERTER_SOURCES
	xercesc/util/Transcoders/MacOSUnicodeConverter/MacOSUnicodeConverter.cpp
)

set(WINTRANS_HEADERS
	xercesc/util/Transcoders/Win32/Win32TransService.hpp
)

set(WINTRANS_SOURCES
	xercesc/util/Transcoders/Win32/Win32TransService.cpp
)

set(CURL_HEADERS
	xercesc/util/NetAccessors/Curl/CurlNetAccessor.hpp
	xercesc/util/NetAccessors/Curl/CurlURLInputStream.hpp
)

set(CURL_SOURCES
	xercesc/util/NetAccessors/Curl/CurlNetAccessor.cpp
	xercesc/util/NetAccessors/Curl/CurlURLInputStream.cpp
)

set(SOCKETS_HEADERS
	xercesc/util/NetAccessors/Socket/SocketNetAccessor.hpp
	xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.hpp
	xercesc/util/NetAccessors/BinHTTPInputStreamCommon.hpp
)

set(SOCKETS_SOURCES
	xercesc/util/NetAccessors/Socket/SocketNetAccessor.cpp
	xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.cpp
	xercesc/util/NetAccessors/BinHTTPInputStreamCommon.cpp
)

set(CFURL_HEADERS
	xercesc/util/NetAccessors/MacOSURLAccessCF/MacOSURLAccessCF.hpp
	xercesc/util/NetAccessors/MacOSURLAccessCF/URLAccessCFBinInputStream.hpp
)

set(CFURL_SOURCES
	xercesc/util/NetAccessors/MacOSURLAccessCF/MacOSURLAccessCF.cpp
	xercesc/util/NetAccessors/MacOSURLAccessCF/URLAccessCFBinInputStream.cpp
)

set(WINSOCK_HEADERS
	xercesc/util/NetAccessors/WinSock/WinSockNetAccessor.hpp
	xercesc/util/NetAccessors/WinSock/BinHTTPURLInputStream.hpp
	xercesc/util/NetAccessors/BinHTTPInputStreamCommon.hpp
)

set(WINSOCK_SOURCES
	xercesc/util/NetAccessors/WinSock/WinSockNetAccessor.cpp
	xercesc/util/NetAccessors/WinSock/BinHTTPURLInputStream.cpp
	xercesc/util/NetAccessors/BinHTTPInputStreamCommon.cpp
)

set(MSGINMEMORY_HEADERS
	xercesc/util/MsgLoaders/InMemory/InMemMsgLoader.hpp
	xercesc/util/MsgLoaders/InMemory/XercesMessages_en_US.hpp
)

set(MSGINMEMORY_SOURCES
	xercesc/util/MsgLoaders/InMemory/InMemMsgLoader.cpp
)

set(MSGICU_HEADERS
	xercesc/util/MsgLoaders/ICU/ICUMsgLoader.hpp
)

set(MSGICU_SOURCES
	xercesc/util/MsgLoaders/ICU/ICUMsgLoader.cpp
)

set(MSGICU_NODIST_SOURCES
	xercesc_messages.c
	root_res.c
)

set(MSGICONV_HEADERS
	xercesc/util/MsgLoaders/MsgCatalog/MsgCatalogLoader.hpp
	xercesc/util/MsgLoaders/MsgCatalog/XMLMsgCat_Ids.hpp
)

set(MSGICONV_SOURCES
	xercesc/util/MsgLoaders/MsgCatalog/MsgCatalogLoader.cpp
)

set(POSIXFMGR_HEADERS
	xercesc/util/FileManagers/PosixFileMgr.hpp
)

set(POSIXFMGR_SOURCES
	xercesc/util/FileManagers/PosixFileMgr.cpp
)

set(WINFMGR_HEADERS
	xercesc/util/FileManagers/WindowsFileMgr.hpp
)

set(WINFMGR_SOURCES
	xercesc/util/FileManagers/WindowsFileMgr.cpp
)

set(NOTHREADMMGR_HEADERS
	xercesc/util/MutexManagers/NoThreadMutexMgr.hpp
)

set(NOTHREADMMGR_SOURCES
	xercesc/util/MutexManagers/NoThreadMutexMgr.cpp
)

set(POSIXMMGR_HEADERS
	xercesc/util/MutexManagers/PosixMutexMgr.hpp
)

set(POSIXMMGR_SOURCES
	xercesc/util/MutexManagers/PosixMutexMgr.cpp
)

set(WINMMGR_HEADERS
	xercesc/util/MutexManagers/WindowsMutexMgr.hpp
)

set(WINMMGR_SOURCES
	xercesc/util/MutexManagers/WindowsMutexMgr.cpp
)


set(LIBXERCES_SOURCES
	${UTIL_SOURCES}
	${DOM_SOURCES}
	${DOMIMPL_SOURCES}
	${FRAMEWORK_SOURCES}
	${INTERNAL_SOURCES}
	${PARSERS_SOURCES}
	${SAX_SOURCES}
	${SAX2_SOURCES}
	${VALIDATORS_SOURCES}
	${XINCLUDE_SOURCES}
)

set(LIBXERCES_HEADERS
	${UTIL_HEADERS}
	${UTIL_REGX_HEADERS}
	${DOM_HEADERS}
	${DOMIMPL_HEADERS}
	${FRAMEWORK_HEADERS}
	${FRAMEWORK_PSVI_HEADERS}
	${INTERNAL_HEADERS}
	${PARSERS_HEADERS}
	${SAX_HEADERS}
	${SAX2_HEADERS}
	${VALIDATORS_COMMON_HEADERS}
	${VALIDATORS_DATATYPE_HEADERS}
	${VALIDATORS_DTD_HEADERS}
	${VALIDATORS_SCHEMA_HEADERS}
	${XINCLUDE_HEADERS}
)

if(XERCES_USE_NETACCESSOR_CURL)
	list(APPEND LIBXERCES_SOURCES ${CURL_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${CURL_HEADERS})
endif()

if(XERCES_USE_NETACCESSOR_SOCKET)
	list(APPEND LIBXERCES_SOURCES ${SOCKETS_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${SOCKETS_HEADERS})
endif()

if(XERCES_USE_NETACCESSOR_CFURL)
	list(APPEND LIBXERCES_SOURCES ${CFURL_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${CFURL_HEADERS})
endif()

if(XERCES_USE_NETACCESSOR_WINSOCK)
	list(APPEND LIBXERCES_SOURCES ${WINSOCK_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${WINSOCK_HEADERS})
endif()

if(XERCES_USE_TRANSCODER_ICU)
	list(APPEND LIBXERCES_SOURCES ${ICU_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${ICU_HEADERS})
endif()

if(XERCES_USE_TRANSCODER_GNUICONV)
	list(APPEND LIBXERCES_SOURCES ${GNUICONV_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${GNUICONV_HEADERS})
endif()

if(XERCES_USE_TRANSCODER_ICONV)
	list(APPEND LIBXERCES_SOURCES ${ICONV_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${ICONV_HEADERS})
endif()

if(XERCES_USE_TRANSCODER_MACOSUNICODECONVERTER)
	list(APPEND LIBXERCES_SOURCES ${MACOSUNICODECONVERTER_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${MACOSUNICODECONVERTER_HEADERS})
endif()

if(XERCES_USE_TRANSCODER_WINDOWS)
	list(APPEND LIBXERCES_SOURCES ${WINTRANS_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${WINTRANS_HEADERS})
endif()

if(XERCES_USE_MSGLOADER_INMEMORY)
	list(APPEND LIBXERCES_SOURCES ${MSGINMEMORY_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${MSGINMEMORY_HEADERS})
endif()

if(XERCES_USE_MSGLOADER_ICU)
	list(APPEND LIBXERCES_SOURCES ${MSGICU_SOURCES} ${MSGICU_NODIST_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${MSGICU_HEADERS})
endif()

if(XERCES_USE_MSGLOADER_ICONV)
	list(APPEND LIBXERCES_SOURCES ${MSGICONV_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${MSGICONV_HEADERS})
endif()

if(XERCES_USE_MUTEXMGR_NOTHREAD)
	list(APPEND LIBXERCES_SOURCES ${NOTHREADMMGR_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${NOTHREADMMGR_HEADERS})
endif()

if(XERCES_USE_MUTEXMGR_POSIX)
	list(APPEND LIBXERCES_SOURCES ${POSIXMMGR_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${POSIXMMGR_HEADERS})
endif()

if(XERCES_USE_MUTEXMGR_WINDOWS)
	list(APPEND LIBXERCES_SOURCES ${WINMMGR_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${WINMMGR_HEADERS})
endif()

if(XERCES_USE_FILEMGR_POSIX)
	list(APPEND LIBXERCES_SOURCES ${POSIXFMGR_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${POSIXFMGR_HEADERS})
endif()

if(XERCES_USE_FILEMGR_WINDOWS)
	list(APPEND LIBXERCES_SOURCES ${WINFMGR_SOURCES})
	list(APPEND LIBXERCES_HEADERS ${WINFMGR_HEADERS})
endif()


set(LINK_FLAGS)
if(APPLE)
	list(APPEND LINK_FLAGS "-framework CoreFoundation")
	if(NOT IOS)
		list(APPEND LINK_FLAGS "-framework CoreServices")
	endif()
else()
	set_source_files_properties(${LIBXERCES_SOURCES} PROPERTIES COMPILE_FLAGS "-std=c++11")
endif()
string(REPLACE ";" " " LINK_FLAGS_STR "${LINK_FLAGS}")


if(NOT HAVE_STRICMP)
	list(APPEND LIBXERCES_SOURCES "stricmp.c")
	list(APPEND LIBXERCES_HEADERS "stricmp.h")
endif()
if(NOT HAVE_STRNICMP)
	list(APPEND LIBXERCES_SOURCES "strnicmp.c")
	list(APPEND LIBXERCES_HEADERS "strnicmp.h")
endif()


if(ENABLE_STATIC)
	add_library(xerces-c-static STATIC ${LIBXERCES_SOURCES})
	set_target_properties(xerces-c-static PROPERTIES OUTPUT_NAME xerces-c)
	if(LINK_FLAGS_STR)
		set_target_properties(xerces-c-static PROPERTIES LINK_FLAGS "${LINK_FLAGS_STR}")
	endif()
endif()
if(ENABLE_SHARED)
	add_library(xerces-c SHARED ${LIBXERCES_SOURCES})
	if(LINK_FLAGS_STR)
		set_target_properties(xerces-c PROPERTIES LINK_FLAGS "${LINK_FLAGS_STR}")
	endif()
	if(MSVC)
		target_compile_definitions(xerces-c PRIVATE "-DDLL_EXPORT" "-DXERCES_BUILDING_LIBRARY")
		if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/xerces-c.pdb
				DESTINATION ${CMAKE_INSTALL_BINDIR}
				PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
			)
		endif()
	endif()
endif()

if(ENABLE_STATIC)
	install(TARGETS xerces-c-static
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif()
if(ENABLE_SHARED)
	install(TARGETS xerces-c
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif()

install(FILES ${DOM_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/dom
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${FRAMEWORK_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/framework
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${FRAMEWORK_PSVI_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/framework/psvi
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${INTERNAL_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/internal
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${PARSERS_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/parsers
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${SAX_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/sax
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${SAX2_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/sax2
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${UTIL_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/util
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${UTIL_REGX_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/util/regx
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${VALIDATORS_COMMON_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/validators/common
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${VALIDATORS_DATATYPE_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/validators/datatype
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${VALIDATORS_DTD_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/validators/DTD
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${VALIDATORS_SCHEMA_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/validators/schema
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES ${XINCLUDE_HEADERS}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/xinclude
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/../xercesc/util/Xerces_autoconf_config.hpp"
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xercesc/util
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
