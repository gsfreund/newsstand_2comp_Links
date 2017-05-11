#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <iostream>
#include <locale>
#include <string>

using namespace std;

// Input a list of 2-comp links in NEWS format and output their standard forms

#define FIND_STANDARD 1
#define ORIENTATION_SHIFT_ONLY 2
#define OUTPUT_ROTATIONS 3
#define OUTPUT_ROTATIONS_REFLECTIONS 4
#define OUTPUT_SHIFTS 5

bool debug = false;

#define MAXSIZE 33333
char test_config [MAXSIZE];
char current_min [MAXSIZE];
char original [MAXSIZE];

int len;

#define val test_config [i]

#define ROTATIONS "xxxxzyyyyzxxxxzyyyyxzzzzxxzzzzxz"



void rotX () {
  for (int i = 0; i < len; i++) {
      switch (val) {
      case 'N':
        val = 'U';
        break;
      case 'n':
        val = 'u';
        break;
      case 'E':
        val = 'E';
        break;
      case 'e':
        val = 'e';
        break;
      case 'W':
        val = 'W';
        break;
      case 'w':
        val = 'w';
        break;
      case 'S':
        val = 'D';
        break;
      case 's':
        val = 'd';
        break;
      case 'U':
        val = 'S';
        break;
      case 'u':
        val = 's';
        break;
      case 'D':
        val = 'N';
        break;
      case 'd':
        val = 'n';
        break;
      }
  }
}

void rotY () {
  for (int i = 0; i < len; i++) {
      switch (val) {
      case 'N':
        val = 'N';
        break;
      case 'n':
        val = 'n';
        break;
      case 'E':
        val = 'D';
        break;
      case 'e':
        val = 'd';
        break;
      case 'W':
        val = 'U';
        break;
      case 'w':
        val = 'u';
        break;
      case 'S':
        val = 'S';
        break;
      case 's':
        val = 's';
        break;
      case 'U':
        val = 'E';
        break;
      case 'u':
        val = 'e';
        break;
      case 'D':
        val = 'W';
        break;
      case 'd':
        val = 'w';
        break;
      }
  }
}
 

void rotZ () {
  for (int i = 0; i < len; i++) {
      switch (val) {
      case 'N':
        val = 'W';
        break;
      case 'n':
        val = 'w';
        break;
      case 'E':
        val = 'N';
        break;
      case 'e':
        val = 'n';
        break;
      case 'W':
        val = 'S';
        break;
      case 'w':
        val = 's';
        break;
      case 'S':
        val = 'E';
        break;
      case 's':
        val = 'e';
        break;
      case 'U':
        val = 'U';
        break;
      case 'u':
        val = 'u';
        break;
      case 'D':
        val = 'D';
        break;
      case 'd':
        val = 'd';
        break;
      }
  }
}

void reflect () {
  for (int i = 0; i < len; i++) {
      switch (val) {
      case 'N':
        val = 'S';
        break;
      case 'n':
        val = 's';
        break;
      case 'E':
        val = 'W';
        break;
      case 'e':
        val = 'w';
        break;
      case 'W':
        val = 'E';
        break;
      case 'w':
        val = 'e';
        break;
      case 'S':
        val = 'N';
        break;
      case 's':
        val = 'n';
        break;
      case 'U':
        val = 'D';
        break;
      case 'u':
        val = 'd';
        break;
      case 'D':
        val = 'U';
        break;
      case 'd':
        val = 'u';
        break;
      }
  }
}



#define blurt(X) if (wiki) fprintf (stderr, " "); fprintf (stderr, X);

void usage (char const *what) {
    bool wiki = false;

    if (!strcmp (what, "-wiki"))
        wiki = true;
    else if (strlen (what) > 1 && what [0] == '-')
        fprintf(stderr, "\n *** Unknown option: %s\n\n", what);
    else if (strlen (what) > 1)
        fprintf(stderr, "\n *** %s\n\n", what);

    if (wiki) fprintf (stderr, " ");
    fprintf (stderr, "newsstand2comp version %s %s\n \n", __DATE__, __TIME__);
    blurt ("Usage: newsstand2comp [options] NEWSfile-in NEWDfile-out\n");
    blurt ("       newstand2comp [options] NEWSfile-in > NEWSfile-out\n");
    blurt ("       newstand2comp -- [options] < NEWSfile-in > NEWSfile-out\n");
    blurt ("       or\n");
    blurt ("       some-NEWS-string-producing-program | newsstand2comp -- [options] > NEWSfile-out\n");
    blurt ("       where [options] can be:\n");
    blurt ("        -a      set achiral flag to true (default is false)\n");
    blurt ("        -r      outpout all rotations of each input NEWS string\n");
    blurt ("        -rr     output all rotations and all reflections of each input NEWS string\n");
    blurt ("        -o      standardize according to orientation and shifts only\n");
    blurt ("    \n");
    blurt ("        -s      output all shifts of each input NEWS string\n");
    exit (102);
}


FILE *open_file (char const *filename, char const *access) {
    FILE *fp = fopen (filename, access);
    if (!fp) {
        fprintf (stderr," *** Can't open file `%s' for %s access!\n", filename, access);
        fflush (stderr);
        exit (1);
    }

    return fp;
}


int main (int argc, char **argv) {
  FILE *fpin, *fpout;
  bool achiral = false;
  int what_to_do = FIND_STANDARD;
  bool read_from_stdin = false;
  
  if (argc < 2) {
      usage ("");
  } 
  
  int index = 1;

  while (index < argc && argv [index][0] == '-') {
      if (!strcmp (argv [index], "-a")) {
          achiral = true;
      }
      else if (!strcmp (argv [index], "-r")) {
          what_to_do = OUTPUT_ROTATIONS;
      }
      else if (!strcmp (argv [index], "-rr")) {
          what_to_do = OUTPUT_ROTATIONS_REFLECTIONS;
      }
      else if (!strcmp (argv [index], "-o")) {
          what_to_do = ORIENTATION_SHIFT_ONLY;
      }
      else if (!strcmp (argv [index], "--")) 
          read_from_stdin = true;
      else if (!strcmp (argv [index], "-s")) {
          what_to_do = OUTPUT_SHIFTS;
      }
      else
          usage (argv [index]);

          index++;
      }

      
  if (read_from_stdin && index != argc || index < argc - 2)
      usage ("too many arguments!");
      
  if (!read_from_stdin && index == argc)
      usage ("too few arguments!");

  if (index < argc)
      fpin = open_file (argv [index++], "r");
  else 
      fpin = stdin;

  if (index < argc)
      fpout = open_file (argv [index++], "w");
  else
      fpout = stdout;

  int lupin = 1;
  if (achiral)
      lupin = 2;

  char *rotations = (char *) calloc (strlen (ROTATIONS) + 2, sizeof (char));
  strcpy (rotations, ROTATIONS);
  
  while (fgets (original, MAXSIZE - 2, fpin)) {
      //we don't have a fix_news()...what does that do in Rob's code?
      len = strlen (original);
      strcpy (test_config, original);
      strcpy (current_min, original);

      locale loc;
      char c;
      int j = 0;
      while (j < len) {
          c = test_config[j];
      //    if (islower(c,loc))
        //      cout << "c is: " << c << "\n";
        //  j++;  
         //     cout << "c = " << c;
        // hey, append each c to a preallocated c string
      }
        
//     const char test = 'A';
  //   cout << "test = " << test << "\n";
  //    char * split;
    //  printf ("splitting test_config %s into tokens:\n", test_config);
     // split = strtok (test_config,"N");
     // printf ("%s\n", split);
      
      switch (what_to_do) {
      case FIND_STANDARD:
        for (int lup = 0; lup < lupin; lup++) { // lupin is 2 for achiral, 1 otherwise
            for (int r = 0; r < strlen (rotations); r++) {
                switch (rotations [r]) {
                case 'x':
                  rotX ();
                  break;
                case 'y':
                  rotY ();
                  break;
                case 'z':
                  rotZ ();
                  break;
                }
                //compare_orientation ();
             }
           //  if (achiral)
             //    reflect ();
        }

      if (strcmp (original, test_config)) {
          fprintf (stderr, "Not back to original conformation!\n");
          exit (1022);
      }

      fprintf (fpout, "%s\n", current_min);
      break;

      case OUTPUT_ROTATIONS:
        fprintf (fpout, "%s\n", original);
        for (int r = 0; r < strlen (rotations); r++) {
            switch (rotations [r]) {
            case 'x':
              rotX ();
              break;
            case 'y':
              rotY ();
              break;
            case 'z':
              rotZ ();
              break;
            } 
            fprintf (fpout, "%s\n", test_config);
        } 
        break;

      case OUTPUT_SHIFTS:
        cout << "it works!!\n";
        break;
        
     }
  }
  
  if (fpin != stdin)
      fclose (fpin);
  if (fpout != stdout)
      fclose (fpout);

  return 0;

}

