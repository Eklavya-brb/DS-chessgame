#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BOARD_SIZE 8

typedef enum { EMPTY, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING } PieceType;
typedef enum { NONE, WHITE, BLACK } Color;

typedef struct {
    PieceType type;
    Color color;
} Piece;

typedef struct MoveNode {
    int sr, sc, dr, dc;
    Piece moved;
    Piece captured;
    struct MoveNode* next;
} MoveNode;

Piece board[BOARD_SIZE][BOARD_SIZE];
MoveNode* moveHistory = NULL;

char getPieceChar(Piece piece) {
    char c = ' ';
    switch (piece.type) {
        case PAWN: c = 'P'; break;
        case ROOK: c = 'R'; break;
        case KNIGHT: c = 'N'; break;
        case BISHOP: c = 'B'; break;
        case QUEEN: c = 'Q'; break;
        case KING: c = 'K'; break;
        default: c = '.';
    }
    return piece.color == WHITE ? c : c; //tolower(c);
}

void initializeBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = (Piece){ EMPTY, NONE };
        }
    }

    // Pawns
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[1][i] = (Piece){ PAWN, BLACK };
        board[6][i] = (Piece){ PAWN, WHITE };
    }

    // Rooks
    board[0][0] = board[0][7] = (Piece){ ROOK, BLACK };
    board[7][0] = board[7][7] = (Piece){ ROOK, WHITE };

    // Knights
    board[0][1] = board[0][6] = (Piece){ KNIGHT, BLACK };
    board[7][1] = board[7][6] = (Piece){ KNIGHT, WHITE };

    // Bishops
    board[0][2] = board[0][5] = (Piece){ BISHOP, BLACK };
    board[7][2] = board[7][5] = (Piece){ BISHOP, WHITE };

    // Queens
    board[0][3] = (Piece){ QUEEN, BLACK };
    board[7][3] = (Piece){ QUEEN, WHITE };

    // Kings
    board[0][4] = (Piece){ KING, BLACK };
    board[7][4] = (Piece){ KING, WHITE };
}

void printBoard() {
    printf("\n  a b c d e f g h\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", getPieceChar(board[i][j]));
        }
        printf("%d\n", 8 - i);
    }
    printf("  a b c d e f g h\n\n");
}

bool isInsideBoard(int r, int c) {
    return r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE;
}

bool isLegalMove(int sr, int sc, int dr, int dc, Color turn) {
    if (!isInsideBoard(sr, sc) || !isInsideBoard(dr, dc)) return false;
    Piece piece = board[sr][sc];
    if (piece.color != turn) return false;
    if (board[dr][dc].color == turn) return false;

    int dx = dr - sr, dy = dc - sc;
    switch (piece.type) {
        case PAWN:
            if (piece.color == WHITE) {
                if (dx == -1 && dy == 0 && board[dr][dc].type == EMPTY) return true;
                if (sr == 6 && dx == -2 && dy == 0 && board[dr][dc].type == EMPTY && board[sr - 1][dc].type == EMPTY) return true;
                if (dx == -1 && abs(dy) == 1 && board[dr][dc].color == BLACK) return true;
            } else {
                if (dx == 1 && dy == 0 && board[dr][dc].type == EMPTY) return true;
                if (sr == 1 && dx == 2 && dy == 0 && board[dr][dc].type == EMPTY && board[sr + 1][dc].type == EMPTY) return true;
                if (dx == 1 && abs(dy) == 1 && board[dr][dc].color == WHITE) return true;
            }
            break;
        case ROOK:
            if (dx == 0 || dy == 0) return true;
            break;
        case KNIGHT:
            if ((abs(dx) == 2 && abs(dy) == 1) || (abs(dx) == 1 && abs(dy) == 2)) return true;
            break;
        case BISHOP:
            if (abs(dx) == abs(dy)) return true;
            break;
        case QUEEN:
            if (dx == 0 || dy == 0 || abs(dx) == abs(dy)) return true;
            break;
        case KING:
            if (abs(dx) <= 1 && abs(dy) <= 1) return true;
            break;
        default:
            break;
    }
    return false;
}

void recordMove(int sr, int sc, int dr, int dc, Piece moved, Piece captured) {
    MoveNode* newMove = (MoveNode*)malloc(sizeof(MoveNode));
    newMove->sr = sr; newMove->sc = sc;
    newMove->dr = dr; newMove->dc = dc;
    newMove->moved = moved;
    newMove->captured = captured;
    newMove->next = moveHistory;
    moveHistory = newMove;
}

bool movePiece(int sr, int sc, int dr, int dc, Color turn) {
    if (!isLegalMove(sr, sc, dr, dc, turn)) return false;
    Piece moved = board[sr][sc];
    Piece captured = board[dr][dc];
    recordMove(sr, sc, dr, dc, moved, captured);
    board[dr][dc] = moved;
    board[sr][sc] = (Piece){ EMPTY, NONE };
    return true;
}

void printMoveHistory() {
    MoveNode* temp = moveHistory;
    printf("\nMove History (latest first):\n");
    while (temp != NULL) {
        char col1 = 'a' + temp->sc;
        char row1 = '8' - temp->sr;
        char col2 = 'a' + temp->dc;
        char row2 = '8' - temp->dr;
        printf("%c%c -> %c%c [%c]\n", col1, row1, col2, row2, getPieceChar(temp->moved));
        temp = temp->next;
    }
}

void freeMoveHistory() {
    MoveNode* temp;
    while (moveHistory) {
        temp = moveHistory;
        moveHistory = moveHistory->next;
        free(temp);
    }
}

void showDeveloperInfo() {
    printf("\n--- Developer Information ---\n");
    printf("Project: Text-based 2-player Chess Game in C\n");
    printf("Developed by: Eklavya Kuldiya\n");
    printf("UID: 24BCD10041\n");
    printf("Data Structures Used: Linked List\n");
}

void playGame() {
    Color turn = WHITE;
    char input[10];

    while (1) {
        printBoard();
        printf("%s's move (e.g., e2e4 or type 'exit'): ", turn == WHITE ? "White" : "Black");
        scanf("%s", input);

        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "history") == 0) {
            printMoveHistory();
            continue;
        }

        if (strlen(input) != 4) {
            printf("Invalid input. Try again.\n");
            continue;
        }

        int sr = 8 - (input[1] - '0');
        int sc = input[0] - 'a';
        int dr = 8 - (input[3] - '0');
        int dc = input[2] - 'a';

        if (movePiece(sr, sc, dr, dc, turn)) {
            turn = (turn == WHITE) ? BLACK : WHITE;
        } else {
            printf("Illegal move. Try again.\n");
        }
    }
}

int main() {
    int choice;

    while (1) {
        printf("\n--- Chess Game Menu ---\n");
        printf("1. Start New Game\n");
        printf("2. View Move History\n");
        printf("3. Exit\n");
        printf("4. Developer Info\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                initializeBoard();
                playGame();
                break;
            case 2:
                printMoveHistory();
                break;
            case 3:
                freeMoveHistory();
                printf("Thanks for playing!\n");
                return 0;
            case 4:
                showDeveloperInfo();
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
