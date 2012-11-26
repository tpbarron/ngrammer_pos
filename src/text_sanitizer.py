import nltk

tokenizer = nltk.data.load('tokenizers/punkt/english.pickle')

MONEY_CNN_COM_DIRECTORY = ["cnn","/home/trevor/Documents/cs/workspaces/cppprojects/ngrammer_pos/crawl_files_cnn/"]
ECONOMIST_COM_DIRECTORY = ["economist","/home/trevor/Documents/cs/workspaces/cppprojects/ngrammer_pos/crawl_files_economist/"]
DIRS = [MONEY_CNN_COM_DIRECTORY, ECONOMIST_COM_DIRECTORY]

def format_files(directories):
	for d in directories:
		url_arr = read_urls(d[1])
		read_articles(url_arr, d)
		print "Finished with " + d[1]
	print "Finished formatting files!"
	
	
def read_urls(directory):
	print "reading urls... " + directory
	url_file = directory + "urls.txt"
	fp_urls = open(url_file, "r")
	url_array = []
	for url in fp_urls:
		url_array.append(directory+url)

	#url_array now holds all the urls as complete paths	
	return url_array

	
def read_articles(url_array, d):
	i = 0
	for url in url_array:
		article = open(url[:-1], "r")
		new_text = parse_article(article)
		nf_str = d[1]+d[0]+str(i)+".txt"
		print "Writing: " + nf_str
		new_article = open(nf_str, "w")
		new_article.write(new_text)
		i+=1
		#replace file with new text

		
def parse_article(fp_article):
	text = ""
	for line in fp_article:
		if line != "\n":
			text += "<p>\n"
			#split sentences
			tokens = tokenizer.tokenize(line[3:-5]) #strip p tags first
			tokens = replace_punc(tokens)
			for i in range(len(tokens)):
				if (tokens[i] != "" and tokens[i] != " " and not tokens[i].isspace()):
					text += "<s>"
					text += tokens[i]
					text += "</s>\n"
				
			text += "</p>"
			text += "\n\n"
	return text
	

import string
def replace_punc(tokens):
	exclude = ['$', '<', '>']
	tokens_stripped = []
	for line in tokens:
		for punct in string.punctuation:
			if not punct in exclude:
				line = line.replace(punct,"").lower()
		tokens_stripped.append(line)
				
	return tokens_stripped


format_files(DIRS)

	
#text = "<p>The program could support an estimated 80 billion pounds in new lending, sources told the Financial Times. Some random text. I am sitting on a plane from Denver to New York. I wish I didn't have to go anywhere this weekend :(.</p>"
#print split_sentences(text)

#read_articles(['/home/trevor/Documents/cs/workspaces/cppprojects/ngrammer_pos/crawl_files_cnn/http:__buzz.money.cnn.com_2012_06_14_bank-of-england-euro-crisis_'])
#f = open('/home/trevor/Desktop/crawl_files/http:__buzz.money.cnn.com_2012_06_14_bank-of-england-euro-crisis_', 'r')
#print parse_article(f)


#url_arr = read_urls(MONEY_CNN_COM_DIRECTORY)
#read_articles(url_arr)
