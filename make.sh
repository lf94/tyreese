gcc -Wall `pkg-config --cflags gtk+-3.0 gtksourceview-3.0 pango` -o t t.c `pkg-config --libs gtk+-3.0 pango` -lgtksourceview-3.0
