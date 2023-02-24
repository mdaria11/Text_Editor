This is a text editor done as an assigment for
the "Data Structure" class. It mainly uses a list
to store the text and queues and stacks for different
commands. 

To use it you need to put your initial text in a file
named "editor.in". For commands you need to put "::i"
before writing the commands in "editor.in" file and 
when you want to add text, close the command block with another "::i", followed
by the text you want to add.

The commands are:
s- save the file after modifications
q - close the editor
b - backspace
u - undo (not fully implemented)
r- redo (not fully implemented)
gl $nr - go to line $nr
gc $nrchar [$line] - go to character $nrchar from current line or $line if the option is specified
dl %nr - delete line $nr
d %nr- delete %nr characters from cursor
dw - delete word
da - delete all words
re $old_word $new_word - replaces the first instance of $old_word from the cursor with the $new_word
ra $old_word $new_word- replace all $ld_word with $new_word after the cursor

An example of editor.in:

      This is a sentence.
      The weather is good today.
      ::i
      gc 8 1
      ::i
      not 
      ::i
      gl 2
      re good rainy
      s
      q
      

The result is going to be in a file named "editor.out" and for the example given it's going to be:

      This is not a sentence.
      The weather is rainy today.
      
