# ptex
A small input method like (not yet a real one) piece for entering LaTeX code with an unsuitable keyboard (e.g. finnish)
# Background
Having been writing a lot of mathematics back in the days, on computers using a swedish/finnish keyboard i bumped into a annoying problem (this is really resurrection of work from the past; 1997!)  Curly braces and the backslash are not in the everyday use places which are taken by the umlaut letters and such.  The cursable alg-gr key is needed to get the all important characters, and this makes fluent typing quite impossible, at least in my experience.  This was my solution back then.

And I still use it today!

#Installing and usage

Compile and put single executable somewhere in the path.  Compilation
has been successful in a few linux installations on x86 hardware.  But
that's it for now.  There could be quite a few problems to fix.

To use in emacs the way I have been doing, put following in `.emacs`
or such:
```
(defun ptex-on-region ()
  "Pipe region through ptex."
  (interactive)
  (shell-command-on-region (region-beginning) (region-end) "ptex" 1 1)
  (exchange-point-and-mark))
```
and bind to desired key.

The whole thing is error prone, but using it on small pieces of text
at the time, it works fine in practise.  At least for me.

Maybe the most valuable idea is the following.  In math variables are
usually single letters while single letter LaTeX commands can be
avoided.  In math mode any then, any one character word is a variable
and other words are commands that getting their backslashes added.  No
need to explicitly type a backslash there!
