/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    CXMLParser.cpp
** AUTHOR(S):   Adrian Kaehler <therealadrian@yahoo.com>
** DESCRIPTION:
**  This code provides a generic XML parser and was adapted from code
**  developed for Redwood by Adrian Kaehler.
*****************************************************************************/

#include "CXMLParser.h"

typedef unsigned long ulong;
typedef unsigned char uchar;

static std::string XMLDecodeString(const std::string& theString)
{
	std::string aNewString;

	//int aUTF8Len = 0;
	//int aUTF8CurVal = 0;

	for (ulong i = 0; i < theString.length(); i++)
	{
		char c = theString[i];

		if (c == '&')
		{
			int aSemiPos = theString.find(';', i);

			if (aSemiPos != -1)
			{
				std::string anEntName = theString.substr(i+1, aSemiPos-i-1);
				i = aSemiPos;
											
				if (anEntName == "lt")
					c = '<';
				else if (anEntName == "amp")
					c = '&';
				else if (anEntName == "gt")
					c = '>';
				else if (anEntName == "quot")
					c = '"';
				else if (anEntName == "apos")
					c = '\'';
				else if (anEntName == "nbsp")
					c = ' ';
				else if (anEntName == "cr")
					c = '\n';
			}
		}				
		
		aNewString += c;
	}

	return aNewString;
}
#if 0
static std::string XMLEncodeString(const std::string& theString)
{
	std::string aNewString;

	bool hasSpace = false;

	for (ulong i = 0; i < theString.length(); i++)
	{
		char c = theString[i];

		if (c == ' ')
		{
			if (hasSpace)
			{
				aNewString += "&nbsp;";
				continue;
			}
			
			hasSpace = true;
		}
		else
			hasSpace = false;

		/*if ((uchar) c >= 0x80)
		{
			// Convert to UTF
			aNewString += (char) (0xC0 | ((c >> 6) & 0xFF));
			aNewString += (char) (0x80 | (c & 0x3F));
		}
		else*/
		{		
			switch (c)
			{
			case '<':
				aNewString += "&lt;";
				break;
			case '&':		
				aNewString += "&amp;";
				break;
			case '>':
				aNewString += "&gt;";
				break;
			case '"':
				aNewString += "&quot;";
				break;
			case '\'':
				aNewString += "&apos;";
				break;
			case '\n':
				aNewString += "&cr;";
				break;
			default:
				aNewString += c;
				break;
			}
		}
	}

	return aNewString;
}
#endif

CXMLParser::CXMLParser()
{
	mFile = NULL;
	mLineNum = 0;
	mAllowComments = false;
	mLastCharWasNewline = true;
}

CXMLParser::~CXMLParser()
{
	if (mFile != NULL)
		fclose(mFile);
}

void CXMLParser::Fail(const std::string& theErrorText)
{
	mHasFailed = true;
	mErrorText = theErrorText;
}

void CXMLParser::Init()
{
	mSection = "";
	mLineNum = 1;
	mHasFailed = false;
	mErrorText = "";	
}

bool CXMLParser::AddAttribute(CXMLElement* theElement, const std::string& theAttributeKey, const std::string& theAttributeValue)
{
	std::pair<CXMLParamMap::iterator,bool> aRet;

	aRet = theElement->mAttributes.insert(CXMLParamMap::value_type(theAttributeKey, theAttributeValue));
	if (!aRet.second)
		aRet.first->second = theAttributeValue;

	if (theAttributeKey != "/")
		theElement->mAttributeIteratorList.push_back(aRet.first);

	return aRet.second;
}

bool CXMLParser::OpenFile(const std::string& theFileName)
{		
	mFile = fopen(theFileName.c_str(), "r");

	if (mFile == NULL)
	{
		mLineNum = 0;
		Fail("Unable to open file " + theFileName);		
		return false;
	}

	mFileName = theFileName.c_str();
	Init();
	return true;
}

void CXMLParser::SetStringSource(const std::string& theString)
{
	Init();

	int aSize = theString.size();

	mBufferedText.resize(aSize);	
	for (int i = 0; i < aSize; i++)
		mBufferedText[i] = theString[aSize - i - 1];	
}

// Encapsulate the grabbing of the next character...
//
char CXMLParser::NextCharacter( int* pVal ) {	

	char c;

	if (mBufferedText.size() > 0) {								
		c = mBufferedText[mBufferedText.size()-1];
		mBufferedText.pop_back();				
		*pVal = 1;
	} else {
		if (mFile != NULL) {
			*pVal = fread(&c, 1, 1, mFile);
		} else {
			*pVal = 0;
		}
	}

	return c;
}

bool CXMLParser::NextElement(CXMLElement* theElement)
{
	for (;;)
	{		
		theElement->mType = CXMLElement::TYPE_NONE;
		theElement->mSection = mSection;
		theElement->mValue = "";
		theElement->mAttributes.clear();			
		theElement->mInstruction.erase();

		bool hasSpace = false;	
		bool inQuote = false;
		bool gotEndQuote = false;

		bool doingAttribute = false;
		bool AttributeVal = false;
		std::string aAttributeKey;
		std::string aAttributeValue;

		std::string aLastAttributeKey;
		
		for (;;)
		{
			// Process character by character

			char c = 0;
			int aVal;
			
			//if (mBufferedText.size() > 0)
			//{								
			//	c = mBufferedText[mBufferedText.size()-1];
			//	mBufferedText.pop_back();				

			//	aVal = 1;
			//}
			//else
			//{
			//	if (mFile != NULL)
			//		aVal = fread(&c, 1, 1, mFile);
			//	else
			//		aVal = 0;
			//}
			
			c = NextCharacter( &aVal );

			if (aVal == 1)
			{
				// If the character was a newline and this character
				// is a single 'tick', then the entire line is a 
				// comment and we should ignore it... just slurp up
				// the chatacters until the next newline.
				//
				if( mLastCharWasNewline==true ) {
					if( c=='\'' ) {
						while( aVal && c!='\n' ) {
							c = NextCharacter( &aVal );
						}
					} else {
						mLastCharWasNewline=false;
					}
				}
				
				bool processChar = false;

				if (c == '\n')
				{
					mLineNum++;
					mLastCharWasNewline = true;			
				}

				if (theElement->mType == CXMLElement::TYPE_COMMENT)
				{
					// Just add text to theElement->mInstruction until we find -->

					std::string* aStrPtr = &theElement->mInstruction;
					
					*aStrPtr += c;					

					int aLen = aStrPtr->length();

					if ((c == '>') && (aLen >= 3) && ((*aStrPtr)[aLen - 2] == '-') && ((*aStrPtr)[aLen - 3] == '-'))
					{
						*aStrPtr = aStrPtr->substr(0, aLen - 3);
						break;
					}
				}
				else if (theElement->mType == CXMLElement::TYPE_INSTRUCTION)
				{
					// Just add text to theElement->mInstruction until we find ?>

					std::string* aStrPtr = &theElement->mValue;

					if ((theElement->mInstruction.length() != 0) || (::isspace(c)))
						aStrPtr = &theElement->mInstruction;
					
					*aStrPtr += c;					

					int aLen = aStrPtr->length();

					if ((c == '>') && (aLen >= 2) && ((*aStrPtr)[aLen - 2] == '?'))
					{
						*aStrPtr = aStrPtr->substr(0, aLen - 2);
						break;
					}
				}
				else
				{
					if (c == '"')
					{
						inQuote = !inQuote;
						if (theElement->mType==CXMLElement::TYPE_NONE || theElement->mType==CXMLElement::TYPE_ELEMENT)
							processChar = true;

						if (!inQuote)
							gotEndQuote = true;
					}
					else if (!inQuote)
					{
						if (c == '<')
						{
							if (theElement->mType == CXMLElement::TYPE_ELEMENT)
							{
								//TODO: Fix buffered text.  Not sure what I meant by that.

								//OLD: mBufferedText = c + mBufferedText;

								mBufferedText.push_back(c);								
								break;
							}

							if (theElement->mType == CXMLElement::TYPE_NONE)
							{
								theElement->mType = CXMLElement::TYPE_START;
							}
							else
							{
								Fail("Unexpected '<'");
								return false;
							}
						}
						else if (c == '>')
						{
							if (theElement->mType == CXMLElement::TYPE_START)
							{	
								bool insertEnd = false;

								if (aAttributeKey == "/")
								{
									// We will get this if we have a space before the />, so we can ignore it
									//  and go about our business now
									insertEnd = true;
								}
								else
								{
									// Probably isn't committed yet
									if (aAttributeKey.length() > 0)
									{										
//										theElement->mAttributes[aLastAttributeKey] = aAttributeValue;

										aAttributeKey = XMLDecodeString(aAttributeKey);
										aAttributeValue = XMLDecodeString(aAttributeValue);

										aLastAttributeKey = aAttributeKey;
										AddAttribute(theElement, aLastAttributeKey, aAttributeValue);

										aAttributeKey = "";
										aAttributeValue = "";
									}

									if (aLastAttributeKey.length() > 0)
									{
										std::string aVal = theElement->mAttributes[aLastAttributeKey];

										int aLen = aVal.length();

										if ((aLen > 0) && (aVal[aLen-1] == '/'))
										{
											// Its an empty element, fake start and end segments
//											theElement->mAttributes[aLastAttributeKey] = aVal.substr(0, aLen - 1);
											
											AddAttribute(theElement, aLastAttributeKey, XMLDecodeString(aVal.substr(0, aLen - 1)));

											insertEnd = true;
										}
									}
									else
									{
										int aLen = theElement->mValue.length();

										if ((aLen > 0) && (theElement->mValue[aLen-1] == '/'))
										{
											// Its an empty element, fake start and end segments
											theElement->mValue = theElement->mValue.substr(0, aLen - 1);
											insertEnd = true;
										}
									}
								}

								// Do we want to fake an ending section?
								if (insertEnd)
								{									
									std::string anAddString = "</" + theElement->mValue + ">";

									int anOldSize = mBufferedText.size();
									int anAddLength = anAddString.length();

									mBufferedText.resize(anOldSize + anAddLength);

									for (int i = 0; i < anAddLength; i++)
										mBufferedText[anOldSize + i] = anAddString[anAddLength - i - 1];																											

									// clear out aAttributeKey, since it contains "/" as its value and will insert
									// it into the element's attribute map.
									aAttributeKey = "";

									//OLD: mBufferedText = "</" + theElement->mValue + ">" + mBufferedText;
								}

								if (mSection.length() != 0)
									mSection += "/";

								mSection += theElement->mValue;								

								break;
							}
							else if (theElement->mType == CXMLElement::TYPE_END)
							{
								int aLastSlash = mSection.rfind('/');
								if ((aLastSlash == -1) && (mSection.length() == 0))
								{
									Fail("Unexpected End");
									return false;
								}

								std::string aLastSectionName = mSection.substr(aLastSlash + 1);
								
								if (aLastSectionName != theElement->mValue)
								{
									Fail("End '" + theElement->mValue + "' Doesn't Match Start '" + aLastSectionName + "'");
									return false;
								}

								if (aLastSlash == -1)
									mSection.erase(mSection.begin(), mSection.end());
								else
									mSection.erase(mSection.begin() + aLastSlash, mSection.end());

								break;
							}
							else
							{
								Fail("Unexpected '>'");
								return false;
							}
						}
						else if ((c == '/') && (theElement->mType == CXMLElement::TYPE_START) && (theElement->mValue == ""))
						{					
							theElement->mType = CXMLElement::TYPE_END;					
						}				
						else if ((c == '?') && (theElement->mType == CXMLElement::TYPE_START) && (theElement->mValue == ""))
						{
							theElement->mType = CXMLElement::TYPE_INSTRUCTION;
						}
						else if (::isspace((uchar) c))
						{
							if (theElement->mValue != "")
								hasSpace = true;

							// It's a comment!
							if ((theElement->mType == CXMLElement::TYPE_START) && (theElement->mValue == "!--"))
								theElement->mType = CXMLElement::TYPE_COMMENT;
						}
						else if ((uchar) c > 32)
						{
							processChar = true;
						}
						else
						{
							Fail("Illegal Character");
							return false;
						}
					} 
					else
					{
						processChar = true;
					}

					if (processChar)
					{
						if (theElement->mType == CXMLElement::TYPE_NONE)
							theElement->mType = CXMLElement::TYPE_ELEMENT;

						if (theElement->mType == CXMLElement::TYPE_START)
						{
							if (hasSpace)
							{
								if ((!doingAttribute) || ((!AttributeVal) && (c != '=')) ||
									((AttributeVal) && ((aAttributeValue.length() > 0) || gotEndQuote)))
								{
									if (doingAttribute)
									{
										aAttributeKey = XMLDecodeString(aAttributeKey);
										aAttributeValue = XMLDecodeString(aAttributeValue);

//										theElement->mAttributes[aAttributeKey] = aAttributeValue;

										AddAttribute(theElement, aAttributeKey, aAttributeValue);

										aAttributeKey = "";
										aAttributeValue = "";

										aLastAttributeKey = aAttributeKey;
									}
									else
									{
										doingAttribute = true;
									}
																
									AttributeVal = false;
								}

								hasSpace = false;
							}

							std::string* aStrPtr = NULL;

							if (!doingAttribute)
							{
								aStrPtr = &theElement->mValue;
							}
							else
							{
								if (c == '=')
								{
									AttributeVal = true;
									gotEndQuote = false;
								}
								else
								{
									if (!AttributeVal)
										aStrPtr = &aAttributeKey;
									else
										aStrPtr = &aAttributeValue;
								}
							}

							if (aStrPtr != NULL)
							{								
								*aStrPtr += c;						
							}
						}
						else
						{
							if (hasSpace)
							{
								theElement->mValue += " ";
								hasSpace = false;
							}
							
							theElement->mValue += c;
						}
					}
				}
			}
			else
			{
				if (theElement->mType != CXMLElement::TYPE_NONE)
					Fail("Unexpected End of File");
					
				return false;
			}			
		}		

		if (aAttributeKey.length() > 0)
		{
			aAttributeKey = XMLDecodeString(aAttributeKey);
			aAttributeValue = XMLDecodeString(aAttributeValue);
//			theElement->mAttributes[aAttributeKey] = aAttributeValue;

			AddAttribute(theElement, aAttributeKey, aAttributeValue);
		}

		theElement->mValue = XMLDecodeString(theElement->mValue);				

		// Ignore comments
		if ((theElement->mType != CXMLElement::TYPE_COMMENT) || mAllowComments)
			return true;
	}
}

bool CXMLParser::HasFailed()
{
	return mHasFailed;
}

std::string CXMLParser::GetErrorText()
{
	return mErrorText;
}

int CXMLParser::GetCurrentLineNum()
{
	return mLineNum;
}

std::string CXMLParser::GetFileName()
{
	return mFileName;
}
