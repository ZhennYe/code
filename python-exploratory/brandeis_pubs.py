# Get all Brandeis publications and plot the trends
# usage: python2 brandeis_pubs.py



from Bio import Entrez
import sys
import json
import string
import matplotlib.pyplot as plt



def ignore_words(pubs, return_words=False):
  # Return words we don't care about
  ignore = ['the', 'a', 'not', 'of', 'for', 'web', 'database', 'and',
            'but', 'that', 'single', 'few', 'several', 'has', 'have', 
            'is', 'was', 'on', 'off', 'to', 'its', 'using', 'with',
            'optimal', 'optimized', 'optimum', 'time', ]
  models = {'mouse': ['mouse', 'mice', 'mus', 'rodent'],
            'fly': ['drosophila', 'melanogaster', 'fly', 'fruit'],
            'ferret': ['ferret'],
            'crab': ['crustacean', 'crab', 'lobster', 'americanus',
                     'cancer', 'americanus', 'borealis', 'panilirus',
                     'interruptus', 'stg', 'stomatogastric']}
  for p in pubs:
    for k in p.keys():
      if type(p[k]) is list:
        p[k] = list(set(p[k]))
        for i in ignore:
          if i in p[k]:
            p[k].pop(p[k].index(i))
  if return_words is False:
    return pubs
  else:
    return pubs, {'ignore': ignore, 'models': models}
  


def remove_punc(pubs):
    # Remove punctuation from all strings
    exclude = set(string.punctuation)
    for p in pubs:
        p['words'] = [''.join(c for c in s if c not in exclude) for s in p['words']]
    return pubs




def format_citations(term, save='~/data/misc/journal_club_2015/all_pubmed.txt'):
  # Make citations easy to check
  retmax = 100000 # Really huge
  Entrez.email = "acsutt0n@gmail.com"
  handle = Entrez.esearch(db="pubmed", term=term, retmax=retmax)
  record = Entrez.read(handle)
  pmids = record["IdList"]
  pubs = []
  for Id in pmids:
    if pmids.index(Id) % 100 == 0:
      print('%i / %i retrieved' %(pmids.index(Id), len(pmids)))
    handle = Entrez.esummary(db="pubmed", id=Id)
    rec = Entrez.read(handle)
    t = {'words': rec[0]["Title"].split(None),
         'authors': [i.split(None)[0] for i in rec[0]["AuthorList"]]}
    try:
      t['year'] = rec[0]["SO"].split(None)[0]
    except:
      try:
        t['year'] = rec[0]["EPubDate"].split(None)[0]
      except:
        t['year'] = rec[0]["PubDate"].split(None)[0]
    # print(t['words'])
    t['words'] = [h.lower() for h in t['words']]
    t['authors'] = [h.lower() for h in t['authors']]
    pubs.append(t)
  # Make sure brandeis is not someone's last name
  for Id in pubs:
    if 'brandeis' in Id['authors']:
      pubs.pop(pubs.index(Id))
  if save is not None:
    json.dump(pubs, open(save, 'w'))
  return pubs



def get_years(pubs):
  prev = None
  for Id in pubs:
    try:
      Id['year'] = int(Id['year'])
    except:
      try:
        if prev in Id['year']:
          Id['year'] = int(prev)
      except:
        print(Id['year'])
        print("Enter year: ")
        h = raw_input()
        prev = h
        Id['year'] = int(h)
  return pubs



def words_by_year(pubs):
  """
  """
  pubs = ignore_words(pubs)
  for p in pubs:
    if type(p['year']) is not int:
      try:
        p['year'] = int( p['year'].split(';')[0] )
      except:
        p['year'] = int( p['year'].split('-')[0] )
  years = [p['year'] for p in pubs]
  years = list(set(years))
  years.sort()
  # Now get the words specific to each year
  words_year = [{} for i in years]
  for p in pubs:
    for w in p['words']:
      #if w not in avoid: # Make sure it's not some stupid obvious word
      if w not in words_year[years.index(p['year'])].keys():
        words_year[years.index(p['year'])][w] = 1
      else:
        words_year[years.index(p['year'])][w] += 1
  return words_year, years



def trends(pubs):
  """
  This returns a dictionary of words as keys and years and 
  word freq for each word.
  """
  w_year, years = words_by_year(pubs)
  keyz = []
  for w in w_year:
    for k in w.keys():
      keyz.append(k)
  keyz = list(set(keyz))
  all_words = {k: [] for k in keyz}
  for w in w_year:
    for word in all_words.keys():
      if word in w.keys():
        all_words[word].append(w[word])
      else:
        all_words[word].append(0)
  return all_words, years
        
  

def show_trends(pubs, thresh=5):
  """
  Start to show some trends.
  """
  all_words, years = trends(pubs)
  # If a word once had > thresh usages, follow it's usage
  words = list(all_words.keys())
  tren = list(all_words.values())
  pop_idx = [w for w in range(len(words)) if sum(tren[w]) > thresh]
  
  




def plot_authors(pubs):
  # Plot which authors are doing well
  return None



def topics(pubs):
  # Plot which subjects/topics receive the most pubs
  return None
         
    
  
  





















#######################
if __name__ == "__main__":
  args = sys.argv
  if len(args) > 1:
    term = args[1]
  else:
    term = "brandeis"
