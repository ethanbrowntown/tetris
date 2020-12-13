#include <gtk/gtk.h>
#include <stdlib.h>

#define WIDTH 10
#define HEIGHT 20
#define CUBE_WIDTH 20
#define EMPTY 0
#define DEAD 1
#define ALIVE 2

static void activate(GtkApplication *, gpointer);
gboolean draw_callback(GtkWidget *, cairo_t *, gpointer);
void add_tetromino(int[][HEIGHT]);
gboolean update_board(void *);
gboolean move_left(void *);
gboolean move_right(void *);
gboolean rotate(void *);
gboolean on_key_press(GtkWidget *, GdkEventKey *, gpointer);

uint32_t g_board[WIDTH][HEIGHT];
GtkWidget *g_darea;
typedef struct block {
  int x;
  int y;
} block_t;
typedef struct tetromino {
  block_t *blocks;
  char type;
} tetromino_t;

void remove_tetromino(tetromino_t **);

tetromino_t *g_tetromino = NULL;

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window;

  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      g_board[i][j] = EMPTY;
    }	    
  }

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), WIDTH * CUBE_WIDTH, HEIGHT * CUBE_WIDTH);

  g_darea = gtk_drawing_area_new();
  gtk_widget_set_size_request(g_darea, WIDTH * CUBE_WIDTH, HEIGHT * CUBE_WIDTH);
  g_signal_connect(G_OBJECT(g_darea), "draw", G_CALLBACK(draw_callback), NULL);

  g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(on_key_press), NULL);

  gtk_container_add(GTK_CONTAINER(window), g_darea);

  g_timeout_add_seconds(1, update_board, g_board);

  gtk_widget_show_all(window);
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user) {
  char *key = gdk_keyval_name(event->keyval);
  if (strcmp(key, "Down") == 0) {
    update_board((void *) g_board);
  }
  else if (strcmp(key, "Left") == 0) {
    move_left((void *) g_board);
  }
  else if (strcmp(key, "Right") == 0) {
    move_right((void *) g_board);
  }
  else if (strcmp(key, "Up") == 0) {
    rotate((void *) g_board);
  }
  return FALSE;
}

void add_tetromino(int board[][HEIGHT]) {
  if (g_tetromino != NULL) {
    return;
  }
  g_tetromino = malloc(sizeof(tetromino_t));
  g_tetromino->blocks = malloc(4 * sizeof(block_t));
  time_t t;
  srand((unsigned) time(&t));
  int random = rand() % 7;
  uint32_t col = 0x00000000;
  switch(random) {
    case 0 :
      (g_tetromino->blocks)[0].x = 0;
      (g_tetromino->blocks)[0].y = 0;
      (g_tetromino->blocks)[1].x = 1;
      (g_tetromino->blocks)[1].y = 0;
      (g_tetromino->blocks)[2].x = 2;
      (g_tetromino->blocks)[2].y = 0;
      (g_tetromino->blocks)[3].x = 3;
      (g_tetromino->blocks)[3].y = 0;
      g_tetromino->type = 'I';
      col = 0x0000FF00;
      break;
    case 1 :
      (g_tetromino->blocks)[0].x = 0;
      (g_tetromino->blocks)[0].y = 0;
      (g_tetromino->blocks)[1].x = 1;
      (g_tetromino->blocks)[1].y = 0;
      (g_tetromino->blocks)[2].x = 2;
      (g_tetromino->blocks)[2].y = 0;
      (g_tetromino->blocks)[3].x = 2;
      (g_tetromino->blocks)[3].y = 1;
      g_tetromino->type = 'J';
      col = 0x00FF0000;
      break;
    case 2 :
      (g_tetromino->blocks)[0].x = 0;
      (g_tetromino->blocks)[0].y = 0;
      (g_tetromino->blocks)[1].x = 1;
      (g_tetromino->blocks)[1].y = 0;
      (g_tetromino->blocks)[2].x = 2;
      (g_tetromino->blocks)[2].y = 0;
      (g_tetromino->blocks)[3].x = 0;
      (g_tetromino->blocks)[3].y = 1;
      g_tetromino->type = 'L';
      col = 0xFF000000;
      break;
    case 3 :
      (g_tetromino->blocks)[0].x = 0;
      (g_tetromino->blocks)[0].y = 0;
      (g_tetromino->blocks)[1].x = 1;
      (g_tetromino->blocks)[1].y = 0;
      (g_tetromino->blocks)[2].x = 0;
      (g_tetromino->blocks)[2].y = 1;
      (g_tetromino->blocks)[3].x = 1;
      (g_tetromino->blocks)[3].y = 1;
      g_tetromino->type = 'O';
      col = 0xFFFFFF00;
      break;
    case 4 :
      (g_tetromino->blocks)[0].x = 0;
      (g_tetromino->blocks)[0].y = 0;
      (g_tetromino->blocks)[1].x = 1;
      (g_tetromino->blocks)[1].y = 0;
      (g_tetromino->blocks)[2].x = 2;
      (g_tetromino->blocks)[2].y = 0;
      (g_tetromino->blocks)[3].x = 1;
      (g_tetromino->blocks)[3].y = 1;
      g_tetromino->type = 'T';
      col = 0xFFFF0000;
      break;
    case 5 :
      (g_tetromino->blocks)[0].x = 0;
      (g_tetromino->blocks)[0].y = 0;
      (g_tetromino->blocks)[1].x = 1;
      (g_tetromino->blocks)[1].y = 0;
      (g_tetromino->blocks)[2].x = 1;
      (g_tetromino->blocks)[2].y = 1;
      (g_tetromino->blocks)[3].x = 2;
      (g_tetromino->blocks)[3].y = 1;
      g_tetromino->type = 'Z';
      col = 0xFF00FF00;
      break;
    case 6 :
      (g_tetromino->blocks)[0].x = 2;
      (g_tetromino->blocks)[0].y = 0;
      (g_tetromino->blocks)[1].x = 1;
      (g_tetromino->blocks)[1].y = 0;
      (g_tetromino->blocks)[2].x = 1;
      (g_tetromino->blocks)[2].y = 1;
      (g_tetromino->blocks)[3].x = 0;
      (g_tetromino->blocks)[3].y = 1;
      g_tetromino->type = 'S';
      col = 0x00FFFF00;
      break;

  }
  for (int i = 0; i < 4; i++) {
    int x =  (g_tetromino->blocks)[i].x;
    int y =  (g_tetromino->blocks)[i].y;
    if (g_board[x][y] & 0xFF == DEAD) {
      printf("Game Over\n");

    }
    g_board[x][y] = col + ALIVE;
  }
}

void remove_tetromino(tetromino_t **tetromino) {
  if (tetromino == NULL) {
    return;
  }
  int x = 0;
  int y = 0;
  for (int i = 0; i < 4; i++) {
    x = ((g_tetromino->blocks)[i]).x;
    y = ((g_tetromino->blocks)[i]).y;
    g_board[x][y] &= 0xFFFFFF00;
    g_board[x][y] += DEAD;
  }
  free((*tetromino)->blocks);
  (*tetromino)->blocks = NULL;
  free(*tetromino);
  *tetromino = NULL; 
  for (int i = 0; i < HEIGHT; i++) {
    int full = 1;
    for (int j = 0; j < WIDTH; j++) {
      full &= g_board[j][i];
    }
    if (full == 1) {
      for (int j = 0; j < WIDTH; j++) {
        g_board[j][i] &= 0xFFFFFF00;
      }
      for (int k = i; k > 0; k--) {
        for (int j = 0; j < WIDTH; j++) {
	  g_board[j][k] = g_board[j][k - 1];
	}
      }
      for (int j = 0; j < WIDTH; j++) {
        g_board[j][0] &= 0xFFFFFF00;
      }
    }
  }
}

gboolean update_board(void *data) {
  int (*board)[HEIGHT] = (int (*)[HEIGHT]) data;
  if (g_tetromino == NULL) {
    add_tetromino(board);
  }
  int x = 0;
  int y = 0;
  for (int i = 0; i < 4; i++) {
    x = ((g_tetromino->blocks)[i]).x;
    y = ((g_tetromino->blocks)[i]).y;
    if (y == HEIGHT - 1) {
      remove_tetromino(&g_tetromino);
      add_tetromino(board);
      i = 5;
    }
    int val = board[x][y + 1] & 0xFF;
    if (val == DEAD) {
      remove_tetromino(&g_tetromino);
      add_tetromino(board);
      i = 5;
    }
  }
  int col = board[((g_tetromino->blocks)[0]).x][((g_tetromino->blocks)[0]).y] & 0xFFFFFF00;
  for (int j = 3; j >= 0; j--) {
    board[((g_tetromino->blocks)[j]).x][((g_tetromino->blocks)[j]).y] = EMPTY;
    (((g_tetromino->blocks)[j]).y)++;
  }
  for (int j = 0; j < 4; j++) {
    board[((g_tetromino->blocks)[j]).x][((g_tetromino->blocks)[j]).y] = EMPTY;
    board[((g_tetromino->blocks)[j]).x][((g_tetromino->blocks)[j]).y] = ALIVE + col; 
  }

  gtk_widget_queue_draw((GtkWidget *) g_darea); 

  return TRUE; 
}

gboolean rotate(void *data) {
  int (*board)[HEIGHT] = (int (*)[HEIGHT]) data;
  if (g_tetromino == NULL) {
    add_tetromino(board);
  }
  switch (g_tetromino->type) {
    case 'O':
      break;
    case 'I':
      if ((g_tetromino->blocks)[0].y == (g_tetromino->blocks)[1].y) {
        int x = (g_tetromino->blocks)[2].x;
	int y = (g_tetromino->blocks)[2].y + 1;
	if (y >= HEIGHT) {
	  y = HEIGHT - 1;
	}
	if (y - 3 < 0) {
          y = 3;
	}
	for (int i = 0; i < 4; i++) {
	  if (board[x][y - i] & 0xFF == DEAD) {
            return TRUE;
	  }
	}
        int col = board[(g_tetromino->blocks)[0].x][(g_tetromino->blocks)[0].y] & 0xFFFFFF00;
	for (int i = 0; i < 4; i++) {
 	  board[(g_tetromino->blocks)[i].x][(g_tetromino->blocks)[i].y] = EMPTY;
	}
	for (int i = 0; i < 4; i++) {
          board[x][y - i] = ALIVE + col;
	  (g_tetromino->blocks)[i].x = x;
	  (g_tetromino->blocks)[i].y = y - i;
	}
      } else {
        int x = (g_tetromino->blocks)[1].x + 1;
	int y = (g_tetromino->blocks)[1].y;
	if (x >= WIDTH) {
          x = WIDTH;
	}
	if (x - 3 < 0) {
          x = 3;
	}
	for (int i = 0; i < 4; i++) {
	  if (board[x - i][y] & 0xFF == DEAD) {
            return TRUE;
	  }
	}
        uint32_t col = board[(g_tetromino->blocks)[0].x][(g_tetromino->blocks)[0].y] & 0xFFFFFF00; 
	for (int i = 0; i < 4; i++) {
 	  board[(g_tetromino->blocks)[i].x][(g_tetromino->blocks)[i].y] = EMPTY;
	}
	for (int i = 0; i < 4; i++) {
	  board[x - i][y] = ALIVE + col;
	  (g_tetromino->blocks)[i].x = x - i;
	  (g_tetromino->blocks)[i].y = y;
	}
      }
      break;
    default :
      {int x = (g_tetromino->blocks)[1].x;
      int y = (g_tetromino->blocks)[1].y;
      if (x - 1 < 0 || x + 1 >= WIDTH || y - 1 < 0 || y + 1 >= HEIGHT) {
        return TRUE;
      }
      for (int i = x - 1; i < x + 2; i++) {
        for (int j = y - 1; j < y + 2; j++) {
          if (board[i][j] & 0xFF == DEAD) {
	    return TRUE;
	  }
	}
      }
      uint32_t col = board[x][y] & 0xFFFFFF00;
      for (int i = x - 1; i < x + 2; i++) {
        for (int j = y - 1; j < y + 2; j++) {
          board[i][j] = EMPTY;
	}
      }
      for (int i = 0; i < 4; i++) {
        int x2 = (g_tetromino->blocks)[i].x;
        int y2 = (g_tetromino->blocks)[i].y;
	if (x2 < x && y2 > y) {
	  (g_tetromino->blocks)[i].y = y - 1;
	}
	else if (x2 < x && y2 == y) {
          (g_tetromino->blocks)[i].y = y - 1;
          (g_tetromino->blocks)[i].x = x;
	}
	else if (x2 < x && y2 < y) {
	  (g_tetromino->blocks)[i].x = x + 1;
	}
	else if (x2 == x && y2 < y) {
	  (g_tetromino->blocks)[i].x = x + 1;
	  (g_tetromino->blocks)[i].y = y;
	}
        else if (x2 > x && y2 < y) {
	  (g_tetromino->blocks)[i].y = y + 1;
	}
	else if (x2 > x && y2 == y) {
	  (g_tetromino->blocks)[i].x = x;
	  (g_tetromino->blocks)[i].y = y + 1;
	}
	else if (x2 > x && y2 > y) {
	  (g_tetromino->blocks)[i].x = x - 1;
	}
	else if (x2 == x && y2 > y) {
          (g_tetromino->blocks)[i].x = x - 1;
          (g_tetromino->blocks)[i].y = y;
	}
	x2 = (g_tetromino->blocks)[i].x;
        y2 = (g_tetromino->blocks)[i].y;
	board[x2][y2] = EMPTY;
	board[g_tetromino->blocks[i].x][g_tetromino->blocks[i].y] = ALIVE + col; 
      }}
  }

  gtk_widget_queue_draw((GtkWidget *) g_darea); 

  return TRUE;
}

gboolean move_left(void *data) {
  if (g_tetromino == NULL) {
    return FALSE;
  }
  int (*board)[HEIGHT] = (int (*)[HEIGHT]) data;
  
  int x = 0;
  int y = 0;
  for (int i = 0; i < 4; i++) {
    x = (g_tetromino->blocks)[i].x;
    y = (g_tetromino->blocks)[i].y;
    if (x == 0) {
      return FALSE;
    }
    int val = board[x - 1][y] & 0xFF;
    if (val == DEAD) {
      return FALSE;
    }
  }
  int col = board[((g_tetromino->blocks)[0]).x][((g_tetromino->blocks)[0]).y] & 0xFFFFFF00; 
  for (int j = 0; j < 4; j++) {
    board[((g_tetromino->blocks)[j]).x][((g_tetromino->blocks)[j]).y] = EMPTY;
    (((g_tetromino->blocks)[j]).x)--;
  }
  for (int j = 0; j < 4; j++) {
    board[((g_tetromino->blocks)[j]).x][((g_tetromino->blocks)[j]).y] = ALIVE + col;
  }


  gtk_widget_queue_draw((GtkWidget *) g_darea); 

  return TRUE; 
}

gboolean move_right(void *data) {
  int (*board)[HEIGHT] = (int (*)[HEIGHT]) data;
  if (g_tetromino == NULL) {
    return FALSE;
  }
  int x = 0;
  int y = 0;
  for (int i = 0; i < 4; i++) {
    x = (g_tetromino->blocks)[i].x;
    y = (g_tetromino->blocks)[i].y;
    if (x == WIDTH - 1) {
      return FALSE;
    }
    int val = board[x + 1][y] & 0xFF;
    if (val == DEAD) {
      return FALSE;
    }
  }
  int col = board[((g_tetromino->blocks)[0]).x][((g_tetromino->blocks)[0]).y] & 0xFFFFFF00;
  for (int j = 3; j >= 0; j--) {
    board[((g_tetromino->blocks)[j]).x][((g_tetromino->blocks)[j]).y] = EMPTY;
    (((g_tetromino->blocks)[j]).x)++;
  }
  for (int j = 3; j >= 0; j--) {
    board[((g_tetromino->blocks)[j]).x][((g_tetromino->blocks)[j]).y] += ALIVE + col;
  }


  gtk_widget_queue_draw((GtkWidget *) g_darea); 

  return TRUE; 
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  gtk_render_background(context, cr, 0, 0, WIDTH, HEIGHT);

  cairo_rectangle(cr, 0, 0, CUBE_WIDTH * WIDTH, CUBE_WIDTH * HEIGHT);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_fill(cr);

  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      if (g_board[i][j]) {
	int col = g_board[i][j] >> 8;
	cairo_set_source_rgb(cr, col >> 16, (col >> 8) & 0xFF, col & 0xFF);
      	cairo_rectangle(cr, CUBE_WIDTH * i, CUBE_WIDTH * j, CUBE_WIDTH, CUBE_WIDTH);
	cairo_fill(cr);
      }
    }
  }
  cairo_fill(cr);

  return FALSE;
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
