#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <pango/pango.h>
#include <gtksourceview/gtksource.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcesearchcontext.h>
#include <gtksourceview/gtksourcesearchsettings.h>

#define SCROLLBAR_WIDTH 8
#define STYLE_PATH "./styles/style.css"

typedef struct {
	char *path;
	gchar *data;
	unsigned int size;
} FileContainer;

typedef struct {
	GtkTextIter start, end;
} GtkIterRange;

typedef struct {
	GtkSourceSearchSettings *settings;
	GtkSourceSearchContext *context;
	GtkIterRange range;
	unsigned int times;
} GtkSearch;

typedef struct {
	GtkWidget *root;
	GtkSourceView *view;
	GtkSourceBuffer *buffer;
	GtkSearch search;
	unsigned int mode;
	GtkEntry *tag;
	FileContainer *fc;
} EditorState;

enum {
	NO_MODE,
	MODE_TYPING,
	MODE_SEARCH,
	ALL_MODES
};

void
event_loop(EditorState *state) {
	gtk_widget_show_all(state->root);
	gtk_widget_hide(GTK_WIDGET(state->tag));
	
	gtk_main();
}

FileContainer *
read_file(char *filename) {
	FILE *input = fopen(filename, "r");
	if(!input) {
		return NULL;
	}
	FileContainer *fc = (FileContainer *)malloc(sizeof(FileContainer));
	fc->path = filename;
	fseek(input, 0, SEEK_END);
	fc->size = ftell(input);
	fseek(input, 0, SEEK_SET);
	fc->data = (gchar *) malloc(fc->size);
	int bytes_read = fread(fc->data, 1, fc->size, input);
	fclose(input);
	if(bytes_read != fc->size) {
		free(fc);
		return NULL;
	}
	return fc;
}

gboolean
write_file(FileContainer *fc, GtkTextView *view, GtkTextBuffer *buffer) {
	gtk_widget_set_sensitive(GTK_WIDGET(view), FALSE);
	GtkTextIter start, end;
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	fc->data = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	gtk_text_buffer_set_modified(buffer, FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(view), TRUE);

	GError *error = NULL;
	if(g_file_set_contents(fc->path, fc->data, -1, &error) == FALSE) {
		printf("Failed to write to file: %s\n", error->message);
		g_free(error);
		return FALSE;
	}

	return FALSE;
}

void
toggle_tag(EditorState *state) {
	if(gtk_widget_is_focus(GTK_WIDGET(state->tag)) == FALSE) {
		gtk_widget_show(GTK_WIDGET(state->tag));
		gtk_widget_grab_focus(GTK_WIDGET(state->tag));
	} else {
		gtk_widget_hide(GTK_WIDGET(state->tag));
		gtk_widget_grab_focus(GTK_WIDGET(state->view));
	}
}

void
select_range(EditorState *state, GtkTextIter *start, GtkTextIter *end) {
	gtk_text_buffer_select_range(GTK_TEXT_BUFFER(state->buffer), start, end);
	gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(state->view), gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(state->buffer)), 0.0, TRUE, 0.0, 0.17);
	
}

void
next_occurrence(EditorState *state) {
	GtkTextIter start, end;
	gboolean found = gtk_source_search_context_forward(state->search.context, &state->search.range.end, &start, &end);
	
	if(found) {
		select_range(state, &start, &end);
		state->search.range.start = start;
		state->search.range.end = end;
	} else {
	}
}

void
change_occurrence(EditorState *state) {
	state->mode = MODE_TYPING;
}

void
append_to_occurrence(EditorState *state) {
	state->mode = MODE_TYPING;
}

void
insert_before_occurrence(EditorState *state) {
	state->mode = MODE_TYPING;
}

void
delete_occurrence(EditorState *state) {
}

void
disengage_search_mode(EditorState *state) {
	gtk_source_search_context_set_highlight(state->search.context, FALSE);
	state->mode = NO_MODE;
}

void
execute_command(const gchar cmd, EditorState *state) {
	if(state->search.times == 0)
		state->search.times = 1;
	unsigned int i = 0;
	for(i = 0; i < state->search.times; i++) {
		switch(cmd) {
			case 'n':
				next_occurrence(state);
				break;
			case 'a':
			case 'i':
			case 'd':
			case 'c':
				break;
			default:
				break;
		}
	}
	state->search.times = 0;
}

gboolean
window_key_press(GtkWidget *widget, GdkEvent *event, gpointer data) {
	EditorState *state = (EditorState *) data;
	gchar command;
	GdkEventKey key_pressed = event->key;
	if(key_pressed.state & GDK_CONTROL_MASK) {
		switch(key_pressed.keyval) { 
			case GDK_KEY_q:
				gtk_main_quit();
				return TRUE;
				
			case GDK_KEY_s:
				write_file(
					state->fc, 
					(GtkTextView *) state->view, 
					(GtkTextBuffer *) state->buffer
				);
				gtk_widget_grab_focus(GTK_WIDGET(state->view));
				return TRUE;
			
			case GDK_KEY_t:
				toggle_tag(state);
				return TRUE;
			default:
				break;
		}
	} else if (state->mode == MODE_SEARCH) {
		switch(key_pressed.keyval) {
			case GDK_KEY_0:
				state->search.times *= 10;
				state->search.times += 0;
				return TRUE;
			case GDK_KEY_1:
				state->search.times *= 10;
				state->search.times += 1;
				return TRUE;
			case GDK_KEY_2:
				state->search.times *= 10;
				state->search.times += 2;
				return TRUE;
			case GDK_KEY_3:
				state->search.times *= 10;
				state->search.times += 3;
				return TRUE;
			case GDK_KEY_4:
				state->search.times *= 10;
				state->search.times += 4;
				return TRUE;
			case GDK_KEY_5:
				state->search.times *= 10;
				state->search.times += 5;
				return TRUE;
			case GDK_KEY_6:
				state->search.times *= 10;
				state->search.times += 6;
				return TRUE;
			case GDK_KEY_7:
				state->search.times *= 10;
				state->search.times += 7;
				return TRUE;
			case GDK_KEY_8:
				state->search.times *= 10;
				state->search.times += 8;
				return TRUE;
			case GDK_KEY_9:
				state->search.times *= 10;
				state->search.times += 9;
				return TRUE;
				
			case GDK_KEY_n:
				command = 'n';
				execute_command(command, state);
				return TRUE;
			case GDK_KEY_c:
				command = 'c';
				execute_command(command, state);
				return TRUE;
			case GDK_KEY_a:
				command = 'a';
				execute_command(command, state);
				return TRUE;
			case GDK_KEY_i:
				command = 'i';
				execute_command(command, state);
				return TRUE;
			case GDK_KEY_d:
				command = 'd';
				execute_command(command, state);
				return TRUE;
			case GDK_KEY_q:
				disengage_search_mode(state);
				return TRUE;
			default:
				return TRUE;
		}
	} else if (state->mode == MODE_TYPING) {
		if(key_pressed.state & GDK_CONTROL_MASK) {
			switch(key_pressed.keyval) {
				case GDK_KEY_t:
					state->mode = MODE_SEARCH;
					return TRUE;
			}
		} else {
			//state->search.func(key_pressed.keyval);
			return TRUE;
		}
	}
	return FALSE;
}

void
engage_search_mode(EditorState *state, const gchar *regexp) {
	gtk_source_search_context_set_highlight(state->search.context, TRUE);
	gtk_source_search_settings_set_search_text (state->search.settings, &regexp[1]);
	GtkSourceSearchContext *search_context = gtk_source_search_context_new(state->buffer, state->search.settings);
	GtkTextIter current_position;
	GtkTextMark *insert_mark = gtk_text_buffer_get_mark(GTK_TEXT_BUFFER(state->buffer), "insert");
	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(state->buffer), &current_position, insert_mark);
	state->search.context = search_context;
	state->search.range.end = current_position;
	state->mode = MODE_SEARCH;
	state->search.times = 0;
}


void
go_to_line(EditorState *state, const gchar *linenumber) {
	GtkTextIter start;
	gint line_number = g_ascii_strtoll(&linenumber[1], NULL, 10);
	gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(state->buffer), &start, line_number);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(state->view), &start, 0.0, FALSE, 0.0, 0.5);
	select_range(state, &start, &start);
}


gboolean
execute_cmd(EditorState *state) {
	const gchar *cmd = gtk_entry_get_text(state->tag);
	if(strlen(cmd) < 0)
		return FALSE;
	
	if(cmd[0] == '/') {
		engage_search_mode(state, cmd);
	} else if(cmd[0] == ':') {
		go_to_line(state, cmd);
	}
	
	return TRUE;
}




gboolean
tag_key_press(GtkWidget *widget, GdkEvent *event, gpointer data) {
	EditorState *state = (EditorState *) data;
	GdkEventKey key_pressed = event->key;
	switch(key_pressed.keyval) { 
		case GDK_KEY_Return:
			if(execute_cmd(state)) {
				toggle_tag(state);
			}
			return TRUE;
		default:
			break;
	}
	return FALSE;
}


void
load_css(void) {
	GtkCssProvider *css_provider = gtk_css_provider_get_default();
	GFile *css_file = g_file_new_for_path(STYLE_PATH);
	GError *error = NULL;
	gtk_css_provider_load_from_file(css_provider, css_file, &error);
	g_object_unref(css_file);

	if(error != NULL) {
		printf("CSS parsing error: %s\n", error->message);
	}

	gtk_style_context_add_provider_for_screen(
		gdk_screen_get_default(), 
		(GtkStyleProvider *)css_provider, 
		GTK_STYLE_PROVIDER_PRIORITY_USER
	);
}

void
set_class(GtkWidget *widget, const gchar *class_name) {
	gtk_style_context_add_class(
		gtk_widget_get_style_context(widget), 
		class_name
	);
}

EditorState *
initialize(int *argc, char **argv[]) {
	gtk_init(argc, argv);

	FileContainer* file_container = read_file((*argv)[1]);
	if(file_container == NULL)
		return NULL;

	load_css();

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), file_container->path);
	set_class(window, "window");
	
	GtkBox *root_container = (GtkBox *)gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(root_container, TRUE);
	GtkBox *box = (GtkBox *)gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(box, FALSE);

	GtkWidget *tag = gtk_entry_new();
	set_class(GTK_WIDGET(tag), "tag");
	
	
	GtkScrolledWindow *source_view_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
	set_class(GTK_WIDGET(source_view_window), "editor-window");
	gtk_scrolled_window_set_policy(
		source_view_window, 
		GTK_POLICY_NEVER, 
		GTK_POLICY_ALWAYS
	);
	
	GtkSourceBuffer *source_view_text = gtk_source_buffer_new(NULL);
	GtkTextBuffer *text_buffer = GTK_TEXT_BUFFER(source_view_text);
	gtk_source_buffer_set_highlight_matching_brackets(source_view_text, FALSE);
	gtk_text_buffer_set_text(
		text_buffer, 
		file_container->data, 
		file_container->size
	);
	GtkTextIter start_of_buffer;
	gtk_text_buffer_get_start_iter(text_buffer, &start_of_buffer);
	gtk_text_buffer_place_cursor(text_buffer, &start_of_buffer);
	gtk_text_buffer_set_modified(text_buffer, FALSE);
	
	
	GtkSourceSearchSettings *search_settings = gtk_source_search_settings_new();
	gtk_source_search_settings_set_regex_enabled (search_settings, TRUE);
	
	
	GtkSourceView *source_view = (GtkSourceView *) gtk_source_view_new_with_buffer(source_view_text);
	set_class(GTK_WIDGET(source_view), "editor");
	gtk_text_view_set_left_margin((GtkTextView *)source_view, SCROLLBAR_WIDTH);
	gtk_text_view_set_wrap_mode((GtkTextView *)source_view, GTK_WRAP_CHAR);
	gtk_source_view_set_draw_spaces(source_view, GTK_SOURCE_DRAW_SPACES_TAB);
	gtk_source_view_set_auto_indent(source_view, TRUE);
	gtk_source_view_set_indent_on_tab(source_view, TRUE);
	gtk_source_view_set_show_line_numbers(source_view, TRUE);
	gtk_container_add(GTK_CONTAINER(source_view_window), GTK_WIDGET(source_view));

	gtk_box_pack_start(box, tag, FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(source_view_window), TRUE, TRUE, 0);
	
	gtk_widget_set_size_request(GTK_WIDGET(window), 640, 480);
	gtk_box_pack_start(root_container, GTK_WIDGET(box), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(root_container));

	gtk_widget_grab_focus(GTK_WIDGET(source_view));

	EditorState *editor_state = (EditorState *)malloc(sizeof(EditorState));
	editor_state->root = window;
	editor_state->view = source_view;
	editor_state->buffer = source_view_text;
	editor_state->search.settings = search_settings;
	editor_state->search.context = NULL;
	editor_state->mode = NO_MODE;
	editor_state->tag = GTK_ENTRY(tag);
	editor_state->fc = file_container;
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(window, "key-press-event", G_CALLBACK(window_key_press), editor_state);
	g_signal_connect(tag, "key-press-event", G_CALLBACK(tag_key_press), editor_state);
	
	return editor_state;
}

int
main(int argc, char *argv[]) {
	EditorState *state = initialize(&argc, &argv);
	if(state->root) {
		event_loop(state);
	}
	return 1;
}
