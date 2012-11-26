import nltk

MONEY_CNN_COM_DIRECTORY = ["cnn","/home/trevor/Documents/cs/workspaces/cppprojects/ngrammer_pos/crawl_files_cnn/"]
ECONOMIST_COM_DIRECTORY = ["economist","/home/trevor/Documents/cs/workspaces/cppprojects/ngrammer_pos/crawl_files_economist/"]
DIRS = [MONEY_CNN_COM_DIRECTORY]
#DIRS = [ECONOMIST_COM_DIRECTORY]

CNN_FILE_COUNT = 1445;
ECONOMIST_FILE_COUNT = 1827;

def write_pos_files(directories):
	for d in directories:
		read_articles(d)
		print "Finished with " + d[1]
	print "Finished writing pos files!"
	
	
def read_articles(d):
	f_count = 0
	if (d[0] == "cnn"):
		f_count = CNN_FILE_COUNT
	elif(d[0] == "economist"):
		f_count = ECONOMIST_FILE_COUNT
		
	for i in range(f_count+1):
		f_str = d[1]+d[0]+str(i)+".txt"
		article = open(f_str, "r")
		new_text = pos_article(article)
		nf_str = d[1]+d[0]+str(i)+"_pos.txt"
		print "Writing: " + nf_str
		new_article = open(nf_str, "w")
		new_article.write(new_text)		

def pos_article(fp_article):
	text = ""
	for line in fp_article:
		if line != "\n":
			line = line[:-1]
			if (line == "<p>"):
				text += "<p>|P_START\n"
			elif (line == "</p>"):
				text += "</p>|P_END\n\n"
			else:
				text += "<s>|S_START "
				line = line.replace("<s>", "").replace("</s>", "")
				sentence_pos = part_of_speech(line)
				for w in sentence_pos:
					text += w[0]+"|"+w[1]+" "
				text += "</s>|S_END\n"
	return text
		

def part_of_speech(s):
	#return nltk.pos_tag(nltk.word_tokenize(s))
	s_tokens = nltk.word_tokenize(s)
	return nltk.pos_tag(s_tokens)
	
	
write_pos_files(DIRS)

#read_articles(['/home/trevor/Documents/cs/workspaces/cppprojects/ngrammer_pos/crawl_files_cnn/cnn1.txt'], MONEY_CNN_COM_DIRECTORY)
#f = open('/home/trevor/Documents/cs/workspaces/cppprojects/ngrammer_pos/crawl_files_cnn/cnn1.txt', 'r')
#print pos_article(f)


#url_arr = read_urls(MONEY_CNN_COM_DIRECTORY)
#read_articles(url_arr)
	
	

