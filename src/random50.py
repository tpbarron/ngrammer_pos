
import random

MONEY_CNN_COM_DIRECTORY = ["cnn","/home/trevor/Desktop/ngrammer_pos/crawl_files_cnn/"]
ECONOMIST_COM_DIRECTORY = ["economist","/home/trevor/Desktop/ngrammer_pos/crawl_files_economist/"]
DIRS = [MONEY_CNN_COM_DIRECTORY, ECONOMIST_COM_DIRECTORY]

CNN_FILE_COUNT = 1445;
ECONOMIST_FILE_COUNT = 1827;

def read_file(f_str):
	f = open(f_str)
	f_lines = f.readlines()
	
	random_lines = []
	used_lines = []
	while(len(random_lines) < 50):
		next_int = random.randint(0, CNN_FILE_COUNT)
		while(next_int in used_lines):
			next_int = random.randint(0, CNN_FILE_COUNT)
			
		#print "line: " + str(next_int)
		used_lines.append(next_int)
		next_line = f_lines[next_int]
		#print next_line
		
		if (next_line[0:3] == "<s>"):
			#line is good!
			random_lines.append(next_line)
	
	for line in random_lines:
		print line[:-1]


read_file(MONEY_CNN_COM_DIRECTORY[1]+"/pos/cat_cnn_pos.txt")	
#write_pos_files(DIRS)

#read_articles(['/home/trevor/Documents/cs/workspaces/cppprojects/ngrammer_pos/crawl_files_cnn/cnn1.txt'], MONEY_CNN_COM_DIRECTORY)
#f = open('/home/trevor/Documents/cs/workspaces/cppprojects/ngrammer_pos/crawl_files_cnn/cnn1.txt', 'r')
#print pos_article(f)


#url_arr = read_urls(MONEY_CNN_COM_DIRECTORY)
#read_articles(url_arr)
	
	

