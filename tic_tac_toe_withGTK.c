#include <stdbool.h>
#include <stdio.h>
#include <gtk/gtk.h>

// Game board dimensions
#define N 3

// Cell states
#define EMPTY 0
#define PLAYER1 1
#define PLAYER2 2

typedef struct {
    int board[N][N];
    int current_player;
} TicTacToeGame;

TicTacToeGame game; // Declare a global game instance

bool gameActive = true; // Track the game's status

// Define the maximum number of moves to store in history
#define MAX_HISTORY_SIZE 50

// Structure to represent a move in the game
typedef struct {
    int row;
    int col;
} GameMove;

GameMove history[MAX_HISTORY_SIZE];
int historyIndex = 0;
GtkWidget *grid; // Declare grid globally
GtkTextView *textview;
GtkTextBuffer *textbuffer;

// Function to add a move to the history
void addMoveToHistory(int row, int col) {
    if (historyIndex < MAX_HISTORY_SIZE - 1) {
        history[historyIndex].row = row;
        history[historyIndex].col = col;
        historyIndex++;
    }
}

// Function to undo the last move
static void undoLastMove() {
    if (historyIndex > 0) {
        historyIndex--;
        int row = history[historyIndex].row;
        int col = history[historyIndex].col;
        game.board[row][col] = EMPTY;
        GtkWidget *button = GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(grid), col, row));
        gtk_button_set_label(GTK_BUTTON(button), " ");
        game.current_player = (game.current_player == PLAYER1) ? PLAYER2 : PLAYER1;
        gameActive = true;
    }
}

// Function to check for a win
int checkForWin(const TicTacToeGame *game) {
    int current_player = game->current_player;
    const int (*board)[N] = game->board;
    for (int i = 0; i < N; i++) {
        if (board[i][0] == current_player && board[i][1] == current_player && board[i][2] == current_player) {
            return current_player;
        }
        if (board[0][i] == current_player && board[1][i] == current_player && board[2][i] == current_player) {
            return current_player;
        }
    }

    if (board[0][0] == current_player && board[1][1] == current_player && board[2][2] == current_player) {
        return current_player;
    }

    if (board[0][2] == current_player && board[1][1] == current_player && board[2][0] == current_player) {
        return current_player;
    }

    return 0; // No win yet
}
void updateGameHistory();
// Function to handle button clicks
static void on_button_clicked(GtkWidget *button, gpointer user_data) {
    if (!gameActive) {
        return; // Game is already won, do nothing
    }

    int row, col;
    sscanf(gtk_widget_get_name(button), "%d,%d", &row, &col);

    if (game.board[row][col] == EMPTY) {
        addMoveToHistory(row, col);

        game.board[row][col] = game.current_player;
        if (game.current_player == PLAYER1) {
            gtk_button_set_label(GTK_BUTTON(button), "X");
        } else {
            gtk_button_set_label(GTK_BUTTON(button), "O");
        }

        int win = checkForWin(&game);
        if (win == PLAYER1) {
            // Player 1 wins
            printf("Player 1 wins!\n");
            gameActive = false; // Set the game as not active
        } else if (win == PLAYER2) {
            // Player 2 wins
            printf("Player 2 wins!\n");
            gameActive = false; // Set the game as not active
        } else {
            // Check for a draw
            int is_draw = 1;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    if (game.board[i][j] == EMPTY) {
                        is_draw = 0;
                        break;
                    }
                }
                if (!is_draw) {
                    break;
                }
            }
            if (is_draw) {
                // It's a draw
                printf("It's a draw!\n");
                gameActive = false; // Set the game as not active
            }
        }

        game.current_player = (game.current_player == PLAYER1) ? PLAYER2 : PLAYER1;
        updateGameHistory(); // Update game history when a move is made
    }
}

// Function to reset the game
static void on_restart_clicked(GtkWidget *button, gpointer user_data) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            game.board[i][j] = EMPTY;

            // Find the corresponding button and set its label to " "
            GtkWidget *child = gtk_grid_get_child_at(GTK_GRID(grid), j, i);
            gtk_button_set_label(GTK_BUTTON(child), " ");
        }
    }
    game.current_player = PLAYER1;
    gameActive = true; // Set the game as active
    historyIndex = 0; // Clear the game history when the game is restarted
    updateGameHistory(); // Update game history
}

// Function to undo the last move
static void on_undo_clicked(GtkWidget *button, gpointer user_data) {
    undoLastMove();
    updateGameHistory(); // Update game history when a move is undone
}

// Function to create the main window and buttons
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *restartButton;
    GtkWidget *undoButton;
    GtkWidget *vbox;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            game.board[i][j] = EMPTY;
        }
    }
    game.current_player = PLAYER1;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Tic Tac Toe");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 300);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(vbox), grid);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            GtkWidget *button = gtk_button_new_with_label(" ");
            gtk_widget_set_name(button, g_strdup_printf("%d,%d", i, j));
            gtk_widget_set_size_request(button, 30, 30); // Set the size for the buttons
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_button_clicked), NULL);
            gtk_grid_attach(GTK_GRID(grid), button, j, i, 1, 1);
        }
    }

    restartButton = gtk_button_new_with_label("Restart");
    g_signal_connect(G_OBJECT(restartButton), "clicked", G_CALLBACK(on_restart_clicked), restartButton);
    gtk_widget_set_size_request(restartButton, 50, 30); // Set the size for the "Restart" button

    undoButton = gtk_button_new_with_label("Undo");
    g_signal_connect(G_OBJECT(undoButton), "clicked", G_CALLBACK(on_undo_clicked), undoButton);
    gtk_widget_set_size_request(undoButton, 50, 30); // Set the size for the "Undo" button

    textview = GTK_TEXT_VIEW(gtk_text_view_new());
    textbuffer = gtk_text_view_get_buffer(textview);
    gtk_text_view_set_editable(textview, FALSE);  // Make the text view read-only

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(textview));

    GtkWidget *historyLabel = gtk_label_new("Game History:");

    GtkWidget *vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox2), historyLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), scrolled_window, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), vbox2, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), restartButton, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), undoButton, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

// Function to update and display the game history in the text view
void updateGameHistory() {
    GtkTextIter iter;

    // Clear the existing text
    gtk_text_buffer_set_text(textbuffer, "", -1);

    // Iterate through the history and append each move to the text buffer
    for (int i = 0; i < historyIndex; i++) {
        gtk_text_buffer_get_end_iter(textbuffer, &iter);
        gchar *move_text = g_strdup_printf("Move %d: Row %d, Column %d\n", i + 1, history[i].row, history[i].col);
        gtk_text_buffer_insert(textbuffer, &iter, move_text, -1);
        g_free(move_text);
    }
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.TicTacToe", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
