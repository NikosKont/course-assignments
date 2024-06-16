#include <stdio.h>

#define offset  182             /* offset of map relative to the char set */
#define def     0               /* default state */
#define M       1
#define N       2               /* special states */
#define m       3
#define n       4

void create_map(char *map[]) {   /* create map */
    map[0] = "'A";              /* 182: Ά */
    map[1] = "\0";              /* 183: . */
    map[2] = "'E";
    map[3] = "'H";
    map[4] = "'I";
    map[5] = "\0";              /* 187: » */
    map[6] = "'O";
    map[7] = "\0";              /* 189: ½ */
    map[8] = "'Y";
    map[9] = "'W";
    map[10] = "i'\"";
    map[11] = "A";
    map[12] = "V";
    map[13] = "G";
    map[14] = "D";
    map[15] = "E";
    map[16] = "Z";
    map[17] = "H";
    map[18] = "8";
    map[19] = "I";
    map[20] = "K";
    map[21] = "L";
    map[22] = "M";
    map[23] = "N";
    map[24] = "KS";
    map[25] = "O";
    map[26] = "P";
    map[27] = "R";
    map[28] = "\0";             /* 210: empty? */
    map[29] = "S";
    map[30] = "T";
    map[31] = "Y";
    map[32] = "F";
    map[33] = "X";
    map[34] = "PS";
    map[35] = "W";
    map[36] = "I\"";
    map[37] = "Y\"";
    map[38] = "a'";
    map[39] = "e'";
    map[40] = "h'";
    map[41] = "i'";
    map[42] = "y'\"";
    map[43] = "a";
    map[44] = "v";
    map[45] = "g";
    map[46] = "d";
    map[47] = "e";
    map[48] = "z";
    map[49] = "h";
    map[50] = "8";
    map[51] = "i";
    map[52] = "k";
    map[53] = "l";
    map[54] = "m";
    map[55] = "n";
    map[56] = "ks";
    map[57] = "o";
    map[58] = "p";
    map[59] = "r";
    map[60] = "s";
    map[61] = "s";
    map[62] = "t";
    map[63] = "y";
    map[64] = "f";
    map[65] = "x";
    map[66] = "ps";
    map[67] = "w";
    map[68] = "i\"";
    map[69] = "y\"";
    map[70] = "o'";
    map[71] = "y'";
    map[72] = "w'";             /* 254: ώ */
}

int state_select(int in) {       /* determines current state */
    switch (in) {
        case 204:
            return M;
        case 205:
            return N;
        case 236:
            return m;
        case 237:
            return n;
        default:
            return def;
    }
}

/*  case M:
    reads next character; if p or P prints B
    else prints M and returns next character  */
int m_cap(int in) {
    int next = getchar();

    if (next == 208 || next == 240) {
        putchar('B');
        return -2;
    }
    else {
        putchar('M');
        return next;
    }
}

/*  case N:
    reads next character; if t or T prints D
    else prints N and returns next character  */
int n_cap(int in) {
    int next = getchar();

    if (next == 212 || next == 244) {
        putchar('D');
        return -2;
    }
    else {
        putchar('N');
        return next;
    }
}

/*  case m:
    reads next character; if p or P prints b
    else prints m and returns next character */
int m_sm(int in) {
    int next = getchar();

    if (next == 208 || next == 240) {
        putchar('b');
        return -2;
    }
    else {
        putchar('m');
        return next;
    }
}

/*  case n:
    reads next character; if t or T prints d
    else prints n and returns next character */
int n_sm(int in) {
    int next = getchar();

    if (next == 212 || next == 244) {
        putchar('d');
        return -2;
    }
    else {
        putchar('n');
        return next;
    }
}

/*  default case:
    if the character is the same in both char sets it is printed as-is
    else prints its mapped character */
int other(int in) {
    int j = 0;
    char c;
    char *map[73];
    create_map(map);

    if (in < 182 || in == 183 || in == 187 || in == 189 || in == 210) {
        putchar(in);
    }
    else {
        while ((c = map[in - offset][j++]) != 0) {
            putchar(c);
        }
    }
    return -2;
}

/*  if in == -2 then input continues,
    if in == EOF program stops,
    if in == a character code then the appropriate function 
    to print next character(s) depending on the current state
    is called and returns the next character  */
int output(int in) {
    const int (*state_functions[])(int in) = {*other, *m_cap, *n_cap, *m_sm, *n_sm};
    int state = state_select(in);
    in = state_functions[state](in);

    if (in == -2 || in == EOF) return in;

    output(in);
    return 0;
}

int main() {
    int in;

    while ((in = getchar()) != EOF) {
        if ((output(in)) == EOF) break;
    }

    return 0;
}