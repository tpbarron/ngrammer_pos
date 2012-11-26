import nltk

def part_of_speech(s):
	print s
	#return nltk.pos_tag(nltk.word_tokenize(s))
	s_tokens = nltk.word_tokenize(s)
	print s_tokens
	return nltk.pos_tag(s_tokens)
