/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    CXMLParser.h
** AUTHOR(S):   Adrian Kaehler <therealadrian@yahoo.com>
** DESCRIPTION:
**  This code provides a generic XML parser and was adapted from code
**  developed for Redwood by Adrian Kaehler.
*****************************************************************************/

#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>

#include <stdio.h>

class CXMLParam
{
public:
	std::string				mKey;
	std::string				mValue;
};

typedef std::map<std::string, std::string>	CXMLParamMap;
typedef std::list<CXMLParamMap::iterator>	CXMLParamMapIteratorList;
typedef std::vector<char> CharVector;

class CXMLElement
{
public:
	enum
	{
		TYPE_NONE,
		TYPE_START,
		TYPE_END,
		TYPE_ELEMENT,
		TYPE_INSTRUCTION,
		TYPE_COMMENT
	};
public:
	
	int						mType;
	std::string				mSection;
	std::string				mValue;
	std::string				mInstruction;
	CXMLParamMap				mAttributes;
	CXMLParamMapIteratorList	mAttributeIteratorList; // stores attribute iterators in their original order
};

class CXMLParser
{
protected:
	std::string				mFileName;
	std::string				mErrorText;
	int						mLineNum;
	FILE*					mFile;
	bool					mHasFailed;
	bool					mAllowComments;
	CharVector				mBufferedText;
	bool					mLastCharWasNewline;

	std::string				mSection;

protected:
	void					Fail(const std::string& theErrorText);
	void					Init();

	bool					AddAttribute(CXMLElement* theElement, const std::string& aAttributeKey, const std::string& aAttributeValue);

public:
	CXMLParser();
	virtual ~CXMLParser();

	bool					OpenFile(const std::string& theFilename);
	void					SetStringSource(const std::string& theString);
	bool					NextElement(CXMLElement* theElement);
	std::string				GetErrorText();
	int						GetCurrentLineNum();
	std::string				GetFileName();

	inline void				AllowComments(bool doAllow) { mAllowComments = doAllow; }

	bool					HasFailed();
	bool					EndOfFile();

private:
	char					NextCharacter( int* aVal );
};





