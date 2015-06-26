def hangman(word):
  alpha = 'abcdefghijklmnopqrstuvwxyz'
  sofar = '_'*len(word)
  def show_guess(sofar): # Show the guess
    print('So far, computer has guessed: %s' %sofar)
    return
  def add_char(char, pos, sofar): # Add new character
    newsofar = ''
    for s in range(len(sofar)):
      if s != pos:
        newsofar = newsofar + sofar[s]
      else:
        newsofar = newsofar + char
    return newsofar
  while sofar != word:
    a = alpha[int(np.random.random(1)*26)]
    for char in range(len(sofar)):
      if sofar[char] == '_': # if it hasn't been guessed
        if word[char] == a:
          sofar = add_char(a, char, sofar)
          show_guess(sofar)
          break
    show_guess(sofar)
  return
