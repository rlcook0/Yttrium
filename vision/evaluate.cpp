/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2009, Stanford University
**
** FILENAME:    classifier.cpp
** AUTHOR(S):   Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  An application for scoring classifier outputs against ground truth
**
*****************************************************************************/

#include <iostream>
#include <vector>
#include "objects.h"
#include "CXMLParser.h"
using namespace std;

static const unsigned int numTypes = 5;

static const char * names[] = {
  "mug",
  "scissors",
  "clock",
  "keyboard",
  "stapler",
  "other"
};

string truncate(const char * str);

unsigned nameToType(const char * name);

void processFrame(vector<vector<unsigned> > & confusionMat, vector<unsigned> & tp, vector<unsigned> & fp, vector<unsigned> & fn, const vector<CObject> & m, const vector<CObject> & g);

void usage()
{
  cerr << "Usage: ./score -m <my_output.xml> -g <ground_truth.xml>" << endl;
  cerr << "(filenames may not contain spaces)" << endl;
}

void parse(CXMLParser & p, vector<vector<CObject> > & v);

double overlap(const CObject & a, const CObject & b); // calculates area of intersection divided over (area a + area b - area intersection)

int main(int argc, char ** argv)
{
  if (argc != 5)
    {
      cerr << "Wrong number of arguments." << endl;
      usage();
      return -1;
    }

  CXMLParser outputParser;
  CXMLParser truthParser;

  bool outputLoaded = false;
  bool truthLoaded = false;

  for (int i = 1; i < 5; i++)
    {
      if (!strcmp(argv[i], "-m"))
	{
	  i++;
	  if (i == 5)
	    {
	      cerr << "No file following -m" << endl;
	      usage();
	      return -1;
	    }
	  if (outputLoaded)
	    {
	      cerr << "Output file specified twice" << endl;
	      usage();
	      return -1;
	    }
	  if (!outputParser.OpenFile(argv[i]))
	    {
	      cerr << "Could not open "<<argv[i]<<endl;
	      usage();
	      return -1;
	    }
	  outputLoaded = true;
	}
      else if (!strcmp(argv[i], "-g"))
	{
	  i++;
	  if (i == 5)
	    {
	      cerr << "No file following -g" << endl;
	      usage();
	      return -1;
	    }
	  if (truthLoaded)
	    {
	      cerr << "Truth file specified twice" << endl;
	      usage();
	      return -1;
	    }
	  if (!truthParser.OpenFile(argv[i]))
	    {
	      cerr << "Could not open "<<argv[i]<<endl;
	      usage();
	      return -1;
	    }
	  truthLoaded = true;
	}
      else
	{
	  cerr << "Unrecognized option "<<argv[i]<<endl;
	  usage();
	  return -1;
	}
    }

  if (!truthLoaded)
    {
      cerr << "No ground truth file specified" << endl;
      usage();
      return -1;
    }

  if (!outputLoaded)
    {
      cerr << "No output file specified" << endl;
      usage();
      return -1;
    }


  vector<vector<CObject> > m;
  vector<vector<CObject> > g;

  parse(truthParser,g);
  parse(outputParser,m);

  if (m.size() != g.size())
    {
      cerr << "Output and ground truth files do not contain the same number of frames" << endl;
      cerr << "Output has "<<m.size()<<" frames"<<endl;
      cerr << "Ground truth has "<<g.size()<<" frames"<<endl;
      return -1;
    }


  vector<unsigned> matRow(numTypes+1, 0);
  vector<vector<unsigned> > mat(numTypes+1, matRow); //rowIdx = true label, colIdx = outputted label. idx numTypes for other
  
  vector<unsigned> tp(numTypes, 0);
  vector<unsigned> fp(numTypes, 0);
  vector<unsigned> fn(numTypes, 0);

  for (unsigned i = 0; i < m.size(); i++)
    {
      processFrame(mat, tp, fp, fn, m[i], g[i]);
    }

  cout << "Confusion matrix: (row = true label, col = outputted label) "<<endl;
  
  for (unsigned i = 0; i <= numTypes; i++)
    cout << "\t"<<truncate(names[i]);
  cout << endl;

  for (unsigned i = 0; i <= numTypes; i++)
    {
      cout << truncate(names[i]);
      for (unsigned j = 0; j <= numTypes; j++)
	cout << "\t"<<mat[i][j];
      cout << endl;
    }


  cout << "(The confusion matrix can be helpful for identifying the mistakes that your classifier makes, but it does not affect your score)" << endl;
  cout << endl;
  cout << "Here is how you would score on each of the individual classes: " << endl;


  for (unsigned i = 0; i < numTypes; i++)
    {
      if (tp[i] + fp[i] == 0)
	continue;

      cout << names[i] << ":"<<endl;
      cout << "\ttrue positives: "<<tp[i]<<endl;
      cout << "\tfalse positives: "<<fp[i]<<endl;
      cout << "\tfalse negatives: "<<fn[i]<<endl;

      double p = double(tp[i])/double(tp[i]+fp[i]);
      cout << "\tprecision: "<<p<<endl;
      
      if (tp[i]+fn[i]==0)
	{
	  cout << "\trecall: N/A (no ground truth objects of this type)" <<endl;
	  cout << "\tF1 score: 0"<< endl;
	}
      else
	{
	  double r = double(tp[i])/double(tp[i]+fn[i]);
	  cout << "\trecall: "<<r<<endl;
	  cout << "F1 score: ";
	  if (p+r == 0)
	    cout << 0<< endl;
	  else
	    cout << 2.0*p*r/(p+r) << endl;
	}
    }

  cout << "Overall scoring information: " << endl;

  unsigned ftp = 0;
  unsigned ffp = 0;
  unsigned ffn = 0;

  for (unsigned i = 0; i < numTypes; i++)
    {
      ftp += tp[i];
      ffp += fp[i];
      ffn += fn[i];
    }

  cout << "\ttrue positives: "<<ftp<<endl;
  cout << "\tfalse positives: "<<ffp<<endl;
  cout << "\tfalse negatives: "<<ffn<<endl;

  double p = 0, r = 0;

  if (ftp+ffp == 0)
      cout << "\tprecision: no labels returned, defaulting to 0"<<endl;
  else
    {
      p = double(ftp)/double(ftp+ffp);
      cout << "\tprecision: "<<p<<endl;
    }

  if (ftp+ffn == 0)
    cout << "\trecall: no ground truth labels, defaulting to 0"<<endl;
  else
    {
      r = double(ftp)/double(ftp+ffn);
      cout << "\trecall: "<<r<<endl;
    }

  cout << "\tFINAL F1-SCORE: ";
  
  if (p+r == 0)
    cout << 0;
  else
    cout << 2.0 * p * r / (p + r);

  cout <<endl;

   return 0;
}

void processFrame(vector<vector<unsigned> > & confusionMat, vector<unsigned> & tp, vector<unsigned> & fp, vector<unsigned> & fn, const vector<CObject> & m, const vector<CObject> & g)
{
  //matching here is greedy-- we step through the ground truth
  //objects in order and match each to the best output object
  //that exceeds the minimum overlap threshold. that output object
  //can then never be used again.
  //this is not necessarily the matching that will give the highest
  //score, but as long as the ground truth objects are not close
  //to each other it should be correct. to maintain correctness
  //with tightly packed objects then instead of doing the greedy
  //assignment one should use LP to solve the matching as a max-flow
  //problem.


  vector<bool> used(m.size(), false);

  for (unsigned i = 0; i < g.size(); i++)
    {
      const char * thisName = g[i].label.c_str();

      unsigned thisType = nameToType(thisName);

      double maxOverlap = 0;
      unsigned bestInd = 0;

      for (unsigned j = 0; j < m.size(); j++)
	{
	  if (used[j] || 0 != strcmp(m[j].label.c_str(), thisName))
	    continue;
	  
	  double o = overlap(g[i],m[j]);

	  if (o > maxOverlap)
	    {
	      bestInd = j;
	      maxOverlap = o;
	    }
	}

      if (maxOverlap < 0.6)
	{
	  //no sufficiently overlapping output objects with the correct label were found, this is a false negative
	  confusionMat[thisType][numTypes]++;
	  fn[thisType]++;
	}
      else
	{
	  //true postive
	  tp[thisType]++;
	  confusionMat[thisType][thisType]++;
	  used[bestInd] = true;
	}
    }

  //Having completed the matching, scan for false positives
  for (unsigned i = 0; i < used.size(); i++)
    {
      if (!used[i])
	{
	  const char * thisName = m[i].label.c_str();
	  unsigned thisType = nameToType(thisName);
	  confusionMat[numTypes][thisType]++;
	  fp[thisType]++;
	}
    }
}


unsigned nameToType(const char * name)
{
  for (unsigned i = 0; i < numTypes; i++)
    if (!strcmp(name, names[i]))
      return i;
  cerr << "Unrecognized object name \""<<name<<"\""<<endl;
  exit(-1);
  return 0; //included to suppress compiler warnings
}

void parse(CXMLParser & p, vector<vector<CObject> > & v)
{
  CXMLElement e;

  if (!p.NextElement(&e))
    {
      cerr << "ERROR: "<< p.GetErrorText() << endl;
      exit(-1);
    }

  if ((e.mType != CXMLElement::TYPE_START) || (e.mValue != "ObjectList"))
    {
      cerr << "ERROR: Unexpected beginning element found" << endl;
      exit(-1);
    }

  while (!p.HasFailed())
    {
      if (!p.NextElement(&e))
	{
	  cerr << "ERROR: "<< p.GetErrorText() << endl;
	  exit(-1);
	}

      if (e.mType == CXMLElement::TYPE_END)
	return;

      //read frame element
      if (e.mType == CXMLElement::TYPE_START && e.mValue == "frame")
	{
	  unsigned frameIndex = atoi(e.mAttributes["id"].c_str());

	  if (frameIndex != v.size())
	    {
	      cerr << "Frame "<<v.size()<<" is incorrectly labeled "<<frameIndex<<endl;
	    }

	  int scored = (((frameIndex - 4) % 10) == 0);

	  if (e.mAttributes.find("scored") != e.mAttributes.end())
	    {
	      int thinksItWasScored = atoi(e.mAttributes["scored"].c_str());
	      assert(thinksItWasScored == scored);
	    }

	  vector<CObject> frame;

	  while (!p.HasFailed())
	    {
	      if (!p.NextElement(&e))
		{
		  cerr << "ERROR: "<< p.GetErrorText() << endl;
		  exit(-1);
		}
	     
	      if (e.mValue == "frame" && e.mType == CXMLElement::TYPE_END)
		break;

	      if (e.mValue == "object" && e.mType != CXMLElement::TYPE_END)
		{
		  CObject obj;
		  obj.rect = cvRect(
				    atoi(e.mAttributes["x"].c_str()),
				    atoi(e.mAttributes["y"].c_str()),
				    atoi(e.mAttributes["w"].c_str()),
				    atoi(e.mAttributes["h"].c_str())
				    );

		  if (e.mAttributes.find("label") != e.mAttributes.end())
		    obj.label = e.mAttributes["label"];
		  else
		    {
		      cerr << "An object does not have a label" << endl;
		      exit(-1);
		    }

		  if (scored)
		    frame.push_back(obj);
 
		}
	    }

	  v.push_back(frame);
	}
    }
}


double overlap(const CObject & a, const CObject & b)
{
  double o = a.overlap(b);
  double aa = a.area();
  double ab = b.area();

  assert(aa > 0);
  assert(ab > 0);

  return o / (aa + ab - o);
}

string truncate(const char * str)
{
  string rval(str);

  if (rval.size() > 5)
    {
      rval = rval.substr(0,5);
      rval[4] = '.';
    }

  return rval;
}
