/*
 *************************************************************************
 *
 *                   "DHRYSTONE" Benchmark Program
 *                   -----------------------------
 *
 *  Version:    C, Version 2.1
 *
 *  File:       dhry.h (part 1 of 3)
 *
 *  Date:       May 25, 1988
 *
 *  Author:     Reinhold P. Weicker
 *              Siemens Nixdorf Inf. Syst.
 *              STM OS 32
 *              Otto-Hahn-Ring 6
 *              W-8000 Muenchen 83
 *              Germany
 *                      Phone:    [+49]-89-636-42436
 *                                (8-17 Central European Time)
 *                      UUCP:     weicker@ztivax.uucp@unido.uucp
 *                      Internet: weicker@ztivax.siemens.com
 *
 *              Original Version (in Ada) published in
 *              "Communications of the ACM" vol. 27., no. 10 (Oct. 1984),
 *              pp. 1013 - 1030, together with the statistics
 *              on which the distribution of statements etc. is based.
 *
 *              In this C version, the following C library functions are
 *              used:
 *              - strcpy, strcmp (inside the measurement loop)
 *              - printf, scanf (outside the measurement loop)
 *
 *  Collection of Results:
 *              Reinhold Weicker (address see above) and
 *              
 *              Rick Richardson
 *              PC Research. Inc.
 *              94 Apple Orchard Drive
 *              Tinton Falls, NJ 07724
 *                      Phone:  (201) 834-1378 (9-17 EST)
 *                      UUCP:   ...!uunet!pcrat!rick
 *
 *      Please send results to Rick Richardson and/or Reinhold Weicker.
 *      Complete information should be given on hardware and software
 *      used. Hardware information includes: Machine type, CPU, type and
 *      size of caches; for microprocessors: clock frequency, memory speed
 *      (number of wait states). Software information includes: Compiler
 *      (and runtime library) manufacturer and version, compilation
 *      switches, OS version. The Operating System version may give an
 *      indication about the compiler; Dhrystone itself performs no OS
 *      calls in the measurement loop.
 *
 *      The complete output generated by the program should be mailed
 *      such that at least some checks for correctness can be made.
 *
 *************************************************************************
 *
 *  History:    This version C/2.1 has been made for two reasons:
 *
 *              1) There is an obvious need for a common C version of
 *              Dhrystone, since C is at present the most popular system
 *              programming language for the class of processors
 *              (microcomputers, minicomputers) where Dhrystone is used
 *              most. There should be, as far as possible, only one C
 *              version of Dhrystone such that results can be compared
 *              without restrictions. In the past, the C versions
 *              distributed by Rick Richardson (Version 1.1) and by
 *              Reinhold Weicker had small (though not significant)
 *              differences.
 *
 *              2) As far as it is possible without changes to the
 *              Dhrystone statistics, optimizing compilers should be
 *              prevented from removing significant statements.
 *
 *              This C version has been developed in cooperation with
 *              Rick Richardson (Tinton Falls, NJ), it incorporates many
 *              ideas from the "Version 1.1" distributed previously by
 *              him over the UNIX network Usenet.
 *              I also thank Chaim Benedelac (National Semiconductor),
 *              David Ditzel (SUN), Earl Killian and John Mashey (MIPS),
 *              Alan Smith and Rafael Saavedra-Barrera (UC at Berkeley)
 *              for their help with comments on earlier versions of the
 *              benchmark.
 *
 *  Changes:    In the initialization part, this version follows mostly
 *              Rick Richardson's version distributed via Usenet, not the
 *              version distributed earlier via floppy disk by Reinhold
 *              Weicker. As a concession to older compilers, names have
 *              been made unique within the first 8 characters. Inside the
 *              measurement loop, this version follows the version
 *              previously distributed by Reinhold Weicker.
 *
 *              At several places in the benchmark, code has been added,
 *              but within the measurement loop only in branches that
 *              are not executed. The intention is that optimizing
 *              compilers should be prevented from moving code out of the
 *              measurement loop, or from removing code altogether. Since
 *              the statements that are executed within the measurement
 *              loop have NOT been changed, the numbers defining the
 *              "Dhrystone distribution" (distribution of statements,
 *              operand types and locality) still hold. Except for
 *              sophisticated optimizing compilers, execution times for
 *              this version should be the same as for previous versions.
 *
 *              Since it has proven difficult to subtract the time for the
 *              measurement loop overhead in a correct way, the loop check
 *              has been made a part of the benchmark. This does have
 *              an impact - though a very minor one - on the distribution
 *              statistics which have been updated for this version.
 *
 *              All changes within the measurement loop are described
 *              and discussed in the companion paper "Rationale for
 *              Dhrystone version 2".
 *
 *              Because of the self-imposed limitation that the order and
 *              distribution of the executed statements should not be
 *              changed, there are still cases where optimizing compilers
 *              may not generate code for some statements. To a certain
 *              degree, this is unavoidable for small synthetic
 *              benchmarks. Users of the benchmark are advised to check
 *              code listings whether code is generated for all statements
 *              of Dhrystone.
 *
 *              Version 2.1 is identical to version 2.0 distributed via
 *              the UNIX network Usenet in March 1988 except that it
 *              corrects some minor deficiencies that were found by users
 *              of version 2.0. The only change within the measurement
 *              loop is that a non-executed "else" part was added to the
 *              "if" statement in Func_3, and a non-executed "else" part
 *              removed from Proc_3.
 *
 *************************************************************************
 *
 * Defines:     The following "Defines" are possible:
 *              -DROPT         (default: Not defined)
 *                      As an approximation to what an average C
 *                      programmer might do, the "register" storage class
 *                      is applied (if enabled by -DROPT)
 *                      - for local variables, if they are used
 *                        (dynamically) five or more times
 *                      - for parameters if they are used (dynamically)
 *                        six or more times
 *                      Note that an optimal "register" strategy is
 *                      compiler-dependent, and that "register"
 *                      declarations do not necessarily lead to faster
 *                      execution.
 *              -DNOSTRUCTASSIGN        (default: Not defined)
 *                      Define if the C compiler does not support
 *                      assignment of structures.
 *              -DNOENUMS               (default: Not defined)
 *                      Define if the C compiler does not support
 *                      enumeration types.
 *
 *************************************************************************
 *
 *  Compilation model and measurement (IMPORTANT):
 *
 *  This C version of Dhrystone consists of three files:
 *  - dhry.h (this file, containing global definitions and comments)
 *  - dhry_1.c (containing the code corresponding to Ada package Pack_1)
 *  - dhry_2.c (containing the code corresponding to Ada package Pack_2)
 *
 *  The following "ground rules" apply for measurements:
 *  - Separate compilation
 *  - No procedure merging
 *  - Otherwise, compiler optimizations are allowed but should be
 *    indicated
 *  - Default results are those without register declarations
 *  See the companion paper "Rationale for Dhrystone Version 2" for a more
 *  detailed discussion of these ground rules.
 *
 *  For 16-Bit processors (e.g. 80186, 80286), times for all compilation
 *  models ("small", "medium", "large" etc.) should be given if possible,
 *  together with a definition of these models for the compiler system
 *  used.
 *
 *************************************************************************
 *
 *  Dhrystone (C version) statistics:
 *
 *  [Comment from the first distribution, updated for version 2.
 *   Note that because of language differences, the numbers are slightly
 *   different from the Ada version.]
 *
 *  The following program contains statements of a high level programming
 *  language (here: C) in a distribution considered representative:
 *
 *    assignments                  52 (51.0 %)
 *    control statements           33 (32.4 %)
 *    procedure, function calls    17 (16.7 %)
 *
 *  103 statements are dynamically executed. The program is balanced with
 *  respect to the three aspects:
 *
 *    - statement type
 *    - operand type
 *    - operand locality
 *         operand global, local, parameter, or constant.
 *
 *  The combination of these three aspects is balanced only approximately.
 *
 *  1. Statement Type:
 *  -----------------             number
 *
 *     V1 = V2                     9
 *       (incl. V1 = F(..)
 *     V = Constant               12
 *     Assignment,                 7
 *       with array element
 *     Assignment,                 6
 *       with record component
 *                                --
 *                                34       34
 *
 *     X = Y +|-|"&&"|"|" Z        5
 *     X = Y +|-|"==" Constant     6
 *     X = X +|- 1                 3
 *     X = Y *|/ Z                 2
 *     X = Expression,             1
 *           two operators
 *     X = Expression,             1
 *           three operators
 *                                --
 *                                18       18
 *
 *     if ....                    14
 *       with "else"      7
 *       without "else"   7
 *           executed        3
 *           not executed    4
 *     for ...                     7  |  counted every time
 *     while ...                   4  |  the loop condition
 *     do ... while                1  |  is evaluated
 *     switch ...                  1
 *     break                       1
 *     declaration with            1
 *       initialization
 *                                --
 *                                34       34
 *
 *     P (...)  procedure call    11
 *       user procedure      10
 *       library procedure    1
 *     X = F (...)
 *             function  call      6
 *       user function        5
 *       library function     1
 *                                --
 *                                17       17
 *                                        ---
 *                                        103
 *
 *    The average number of parameters in procedure or function calls
 *    is 1.82 (not counting the function values as implicit parameters).
 *
 *
 *  2. Operators
 *  ------------
 *                          number    approximate
 *                                    percentage
 *
 *    Arithmetic             32          50.8
 *
 *       +                     21          33.3
 *       -                      7          11.1
 *       *                      3           4.8
 *       / (int div)            1           1.6
 *
 *    Comparison             27           42.8
 *
 *       ==                     9           14.3
 *       /=                     4            6.3
 *       >                      1            1.6
 *       <                      3            4.8
 *       >=                     1            1.6
 *       <=                     9           14.3
 *
 *    Logic                   4            6.3
 *
 *       && (AND-THEN)          1            1.6
 *       |  (OR)                1            1.6
 *       !  (NOT)               2            3.2
 * 
 *                           --          -----
 *                           63          100.1
 *
 *
 *  3. Operand Type (counted once per operand reference):
 *  ---------------
 *                          number    approximate
 *                                    percentage
 *
 *     Integer               175        72.3 %
 *     Character              45        18.6 %
 *     Pointer                12         5.0 %
 *     String30                6         2.5 %
 *     Array                   2         0.8 %
 *     Record                  2         0.8 %
 *                           ---       -------
 *                           242       100.0 %
 *
 *  When there is an access path leading to the final operand (e.g. a
 *  record component), only the final data type on the access path is
 *  counted.
 *
 *
 *  4. Operand Locality:
 *  -------------------
 *                                number    approximate
 *                                          percentage
 *
 *     local variable              114        47.1 %
 *     global variable              22         9.1 %
 *     parameter                    45        18.6 %
 *        value                        23         9.5 %
 *        reference                    22         9.1 %
 *     function result               6         2.5 %
 *     constant                     55        22.7 %
 *                                 ---       -------
 *                                 242       100.0 %
 *
 *
 *  The program does not compute anything meaningful, but it is
 *  syntactically and semantically correct. All variables have a value
 *  assigned to them before they are used as a source operand.
 *
 *  There has been no explicit effort to account for the effects of a
 *  cache, or to balance the use of long or short displacements for code
 *  or data.
 *
 *************************************************************************
 */

import std.c.stdio;
import std.c.string;
import std.c.stdlib;
import std.string;


/* Compiler and system dependent definitions: */

const double Mic_secs_Per_Second = 1000000.0;
                /* Berkeley UNIX C returns process times in seconds/HZ */

enum {Ident_1, Ident_2, Ident_3, Ident_4, Ident_5}
alias int Enumeration;
        /* for boolean and enumeration types in Ada, Pascal */

/* General definitions: */

const int StrLen = 30;

alias int     One_Thirty;
alias int     One_Fifty;
alias char    Capital_Letter;
alias int     Boolean;
alias char    Str_30 [StrLen];
alias int     Arr_1_Dim [50];
alias int     Arr_2_Dim [50] [50];

struct record 
{
    record *Ptr_Comp;
    Enumeration    Discr;
    union V {
          struct V1 {
                  Enumeration Enum_Comp;
                  int         Int_Comp;
                  char        Str_Comp [StrLen];
                  }
	  V1 var_1;
          struct V2 {
                  Enumeration E_Comp_2;
                  char        Str_2_Comp [StrLen];
                  }
	  V2 var_2;
          struct V3 {
                  char        Ch_1_Comp;
                  char        Ch_2_Comp;
                  }
	  V3 var_3;
          }
    V variant;
}

alias record Rec_Type;
alias record *Rec_Pointer;


/* Global Variables: */

Rec_Pointer     Ptr_Glob,
                Next_Ptr_Glob;
int             Int_Glob;
Boolean         Bool_Glob;
char            Ch_1_Glob,
                Ch_2_Glob;
int             Arr_1_Glob [50];
int             Arr_2_Glob [50] [50];

char[StrLen] Reg_Define = "Register option selected.";

/* variables for time measurement: */

const int Too_Small_Time = 2;
                /* Measurements should last at least 2 seconds */

double          Begin_Time,
                End_Time,
                User_Time;

double          Microseconds,
                Dhrystones_Per_Second,
                Vax_Mips;

/* end of variables for time measurement */


void main ()
/*****/

  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{
        One_Fifty       Int_1_Loc;
        One_Fifty       Int_2_Loc;
        One_Fifty       Int_3_Loc;
        char            Ch_Index;
        Enumeration     Enum_Loc;
        Str_30          Str_1_Loc;
        Str_30          Str_2_Loc;
        int             Run_Index;
        int             Number_Of_Runs;

        FILE            *Ap;

  /* Initializations */

  if ((Ap = fopen("dhry.res","a+")) == null)
    {
       printf("Can not open dhry.res\n\n");
       exit(1);
    }

  Next_Ptr_Glob = cast(Rec_Pointer) malloc (Rec_Type.sizeof);
  Ptr_Glob = cast(Rec_Pointer) malloc (Rec_Type.sizeof);

  Ptr_Glob.Ptr_Comp                    = Next_Ptr_Glob;
  Ptr_Glob.Discr                       = Ident_1;
  Ptr_Glob.variant.var_1.Enum_Comp     = Ident_3;
  Ptr_Glob.variant.var_1.Int_Comp      = 40;
//  strcpy (Ptr_Glob.variant.var_1.Str_Comp, 
//          "DHRYSTONE PROGRAM, SOME STRING");
//  strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");
  Ptr_Glob.variant.var_1.Str_Comp[] = "DHRYSTONE PROGRAM, SOME STRING";
  Str_1_Loc[] = "DHRYSTONE PROGRAM, 1'ST STRING";

  Arr_2_Glob [8][7] = 10;
        /* Was missing in published program. Without this statement,    */
        /* Arr_2_Glob [8][7] would have an undefined value.             */
        /* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
        /* overflow may occur for this array element.                   */

  printf ("\n");
  printf ("Dhrystone Benchmark, Version 2.1 (Language: D)\n");
  printf ("\n");
  printf ("Please give the number of runs through the benchmark: ");
  {
    int n;
    //scanf ("%d", &n);
    n = 10000000;
    Number_Of_Runs = n;
  }
  printf ("\n");

  printf ("Execution starts, %d runs through Dhrystone\n",Number_Of_Runs);

  /***************/
  /* Start timer */
  /***************/
 
  Begin_Time = dtime();
  
  for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
  {

    Proc_5();
    Proc_4();
      /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
    Int_1_Loc = 2;
    Int_2_Loc = 3;
    //strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
    Str_2_Loc[] = "DHRYSTONE PROGRAM, 2'ND STRING";
    Enum_Loc = Ident_2;
    Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
      /* Bool_Glob == 1 */
    while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
    {
      Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
        /* Int_3_Loc == 7 */
      Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
        /* Int_3_Loc == 7 */
      Int_1_Loc += 1;
    } /* while */
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
      /* Int_Glob == 5 */
    Proc_1 (Ptr_Glob);
    for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
                             /* loop body executed twice */
    {
      if (Enum_Loc == Func_1 (Ch_Index, 'C'))
          /* then, not executed */
        {
        Proc_6 (Ident_1, &Enum_Loc);
        //strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING");
        Str_2_Loc[] = "DHRYSTONE PROGRAM, 3'RD STRING";
        Int_2_Loc = Run_Index;
        Int_Glob = Run_Index;
        }
    }
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Int_2_Loc = Int_2_Loc * Int_1_Loc;
    Int_1_Loc = Int_2_Loc / Int_3_Loc;
    Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
      /* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
    Proc_2 (&Int_1_Loc);
      /* Int_1_Loc == 5 */

  } /* loop "for Run_Index" */

  /**************/
  /* Stop timer */
  /**************/

  End_Time = dtime();

  printf ("Execution ends\n");
  printf ("\n");
  printf ("Final values of the variables used in the benchmark:\n");
  printf ("\n");
  printf ("Int_Glob:            %d\n", Int_Glob);
  printf ("        should be:   %d\n", 5);
  printf ("Bool_Glob:           %d\n", Bool_Glob);
  printf ("        should be:   %d\n", 1);
  printf ("Ch_1_Glob:           %c\n", Ch_1_Glob);
  printf ("        should be:   %c\n", cast(int)'A');
  printf ("Ch_2_Glob:           %c\n", Ch_2_Glob);
  printf ("        should be:   %c\n", cast(int)'B');
  printf ("Arr_1_Glob[8]:       %d\n", Arr_1_Glob[8]);
  printf ("        should be:   %d\n", 7);
  printf ("Arr_2_Glob[8][7]:    %d\n", Arr_2_Glob[8][7]);
  printf ("        should be:   Number_Of_Runs + 10\n");
  printf ("Ptr_Glob.\n");
  printf ("  Ptr_Comp:          %d\n", cast(int) Ptr_Glob.Ptr_Comp);
  printf ("        should be:   (implementation-dependent)\n");
  printf ("  Discr:             %d\n", Ptr_Glob.Discr);
  printf ("        should be:   %d\n", 0);
  printf ("  Enum_Comp:         %d\n", Ptr_Glob.variant.var_1.Enum_Comp);
  printf ("        should be:   %d\n", 2);
  printf ("  Int_Comp:          %d\n", Ptr_Glob.variant.var_1.Int_Comp);
  printf ("        should be:   %d\n", 17);
  printf ("  Str_Comp:          %.*s\n", Ptr_Glob.variant.var_1.Str_Comp);
  printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  printf ("Next_Ptr_Glob.\n");
  printf ("  Ptr_Comp:          %d\n", cast(int) Next_Ptr_Glob.Ptr_Comp);
  printf ("        should be:   (implementation-dependent), same as above\n");
  printf ("  Discr:             %d\n", Next_Ptr_Glob.Discr);
  printf ("        should be:   %d\n", 0);
  printf ("  Enum_Comp:         %d\n", Next_Ptr_Glob.variant.var_1.Enum_Comp);
  printf ("        should be:   %d\n", 1);
  printf ("  Int_Comp:          %d\n", Next_Ptr_Glob.variant.var_1.Int_Comp);
  printf ("        should be:   %d\n", 18);
  printf ("  Str_Comp:          %.*s\n", Next_Ptr_Glob.variant.var_1.Str_Comp);
  printf ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  printf ("Int_1_Loc:           %d\n", Int_1_Loc);
  printf ("        should be:   %d\n", 5);
  printf ("Int_2_Loc:           %d\n", Int_2_Loc);
  printf ("        should be:   %d\n", 13);
  printf ("Int_3_Loc:           %d\n", Int_3_Loc);
  printf ("        should be:   %d\n", 7);
  printf ("Enum_Loc:            %d\n", Enum_Loc);
  printf ("        should be:   %d\n", 1);
  printf ("Str_1_Loc:           %.*s\n", Str_1_Loc);
  printf ("        should be:   DHRYSTONE PROGRAM, 1'ST STRING\n");
  printf ("Str_2_Loc:           %.*s\n", Str_2_Loc);
  printf ("        should be:   DHRYSTONE PROGRAM, 2'ND STRING\n");
  printf ("\n");

  User_Time = End_Time - Begin_Time;

  if (User_Time < Too_Small_Time)
  {
    printf ("Measured time too small to obtain meaningful results\n");
    printf ("Please increase number of runs\n");
    printf ("\n");
  }
  else
  {
    Microseconds = User_Time * Mic_secs_Per_Second 
                        / cast(double) Number_Of_Runs;
    Dhrystones_Per_Second = cast(double) Number_Of_Runs / User_Time;
    Vax_Mips = Dhrystones_Per_Second / 1757.0;

    printf ("Register option selected?  NO\n");
    strcpy(Reg_Define, "Register option not selected.");
    printf ("Microseconds for one run through Dhrystone: ");
    printf ("%7.1lf \n", Microseconds);
    printf ("Dhrystones per Second:                      ");
    printf ("%10.1lf \n", Dhrystones_Per_Second);
    printf ("VAX MIPS rating = %10.3lf \n",Vax_Mips);
    printf ("\n");
  
  fprintf(Ap,"\n");
  fprintf(Ap,"Dhrystone Benchmark, Version 2.1 (Language: D)\n");
  fprintf(Ap,"%.*s\n",Reg_Define);
  fprintf(Ap,"Microseconds for one loop: %7.1lf\n",Microseconds);
  fprintf(Ap,"Dhrystones per second: %10.1lf\n",Dhrystones_Per_Second);
  fprintf(Ap,"VAX MIPS rating: %10.3lf\n",Vax_Mips);
  fclose(Ap);
  
  }
  
}

void Proc_1 (Rec_Pointer Ptr_Val_Par)
/******************/

    /* executed once */
{
      Rec_Pointer Next_Record = Ptr_Val_Par.Ptr_Comp;  
                                        /* == Ptr_Glob_Next */
  /* Local variable, initialized with Ptr_Val_Par.Ptr_Comp,    */
  /* corresponds to "rename" in Ada, "with" in Pascal           */
  
  *Ptr_Val_Par.Ptr_Comp = *Ptr_Glob;
  Ptr_Val_Par.variant.var_1.Int_Comp = 5;
  Next_Record.variant.var_1.Int_Comp 
        = Ptr_Val_Par.variant.var_1.Int_Comp;
  Next_Record.Ptr_Comp = Ptr_Val_Par.Ptr_Comp;
  Proc_3 (&Next_Record.Ptr_Comp);
    /* Ptr_Val_Par.Ptr_Comp.Ptr_Comp 
                        == Ptr_Glob.Ptr_Comp */
  if (Next_Record.Discr == Ident_1)
    /* then, executed */
  {
    Next_Record.variant.var_1.Int_Comp = 6;
    Proc_6 (Ptr_Val_Par.variant.var_1.Enum_Comp, 
           &Next_Record.variant.var_1.Enum_Comp);
    Next_Record.Ptr_Comp = Ptr_Glob.Ptr_Comp;
    Proc_7 (Next_Record.variant.var_1.Int_Comp, 10, 
           &Next_Record.variant.var_1.Int_Comp);
  }
  else /* not executed */
    *Ptr_Val_Par = *Ptr_Val_Par.Ptr_Comp;
} /* Proc_1 */

void Proc_2 (One_Fifty *Int_Par_Ref)
/******************/
    /* executed once */
    /* *Int_Par_Ref == 1, becomes 4 */
{
  One_Fifty  Int_Loc;
  Enumeration   Enum_Loc;

  Int_Loc = *Int_Par_Ref + 10;
  do /* executed once */
    if (Ch_1_Glob == 'A')
      /* then, executed */
    {
      Int_Loc -= 1;
      *Int_Par_Ref = Int_Loc - Int_Glob;
      Enum_Loc = Ident_1;
    } /* if */
  while (Enum_Loc != Ident_1); /* true */
} /* Proc_2 */


void Proc_3 (Rec_Pointer *Ptr_Ref_Par)
/******************/
    /* executed once */
    /* Ptr_Ref_Par becomes Ptr_Glob */

{
  if (Ptr_Glob !=    null)
    /* then, executed */
    *Ptr_Ref_Par = Ptr_Glob.Ptr_Comp;
  Proc_7 (10, Int_Glob, &Ptr_Glob.variant.var_1.Int_Comp);
} /* Proc_3 */

void Proc_4 () /* without parameters */
/*******/
    /* executed once */
{
  Boolean Bool_Loc;

  Bool_Loc = Ch_1_Glob == 'A';
  Bool_Glob = Bool_Loc | Bool_Glob;
  Ch_2_Glob = 'B';
} /* Proc_4 */


void Proc_5 () /* without parameters */
/*******/
    /* executed once */
{
  Ch_1_Glob = 'A';
  Bool_Glob = false;
} /* Proc_5 */


void Proc_6 (Enumeration Enum_Val_Par, Enumeration *Enum_Ref_Par)
/*********************************/
    /* executed once */
    /* Enum_Val_Par == Ident_3, Enum_Ref_Par becomes Ident_2 */
{
  *Enum_Ref_Par = Enum_Val_Par;
  if (! Func_3 (Enum_Val_Par))
    /* then, not executed */
    *Enum_Ref_Par = Ident_4;
  switch (Enum_Val_Par)
  {
    case Ident_1: 
      *Enum_Ref_Par = Ident_1;
      break;
    case Ident_2: 
      if (Int_Glob > 100)
        /* then */
      *Enum_Ref_Par = Ident_1;
      else *Enum_Ref_Par = Ident_4;
      break;
    case Ident_3: /* executed */
      *Enum_Ref_Par = Ident_2;
      break;
    case Ident_4: break;
    case Ident_5: 
      *Enum_Ref_Par = Ident_3;
      break;
  } /* switch */
} /* Proc_6 */


void Proc_7 (One_Fifty Int_1_Par_Val, One_Fifty Int_2_Par_Val, One_Fifty *Int_Par_Ref)
/**********************************************/
    /* executed three times                                      */
    /* first call:      Int_1_Par_Val == 2, Int_2_Par_Val == 3,  */
    /*                  Int_Par_Ref becomes 7                    */
    /* second call:     Int_1_Par_Val == 10, Int_2_Par_Val == 5, */
    /*                  Int_Par_Ref becomes 17                   */
    /* third call:      Int_1_Par_Val == 6, Int_2_Par_Val == 10, */
    /*                  Int_Par_Ref becomes 18                   */
{
  One_Fifty Int_Loc;

  Int_Loc = Int_1_Par_Val + 2;
  *Int_Par_Ref = Int_2_Par_Val + Int_Loc;
} /* Proc_7 */


void Proc_8 (Arr_1_Dim Arr_1_Par_Ref, Arr_2_Dim Arr_2_Par_Ref, int Int_1_Par_Val, int Int_2_Par_Val)
/*********************************************************************/
    /* executed once      */
    /* Int_Par_Val_1 == 3 */
    /* Int_Par_Val_2 == 7 */
{
      One_Fifty Int_Index;
      One_Fifty Int_Loc;

  Int_Loc = Int_1_Par_Val + 5;
  Arr_1_Par_Ref [Int_Loc] = Int_2_Par_Val;
  Arr_1_Par_Ref [Int_Loc+1] = Arr_1_Par_Ref [Int_Loc];
  Arr_1_Par_Ref [Int_Loc+30] = Int_Loc;
  for (Int_Index = Int_Loc; Int_Index <= Int_Loc+1; ++Int_Index)
    Arr_2_Par_Ref [Int_Loc] [Int_Index] = Int_Loc;
  Arr_2_Par_Ref [Int_Loc] [Int_Loc-1] += 1;
  Arr_2_Par_Ref [Int_Loc+20] [Int_Loc] = Arr_1_Par_Ref [Int_Loc];
  Int_Glob = 5;
} /* Proc_8 */


Enumeration Func_1 (Capital_Letter Ch_1_Par_Val, Capital_Letter Ch_2_Par_Val)
/*************************************************/
    /* executed three times                                         */
    /* first call:      Ch_1_Par_Val == 'H', Ch_2_Par_Val == 'R'    */
    /* second call:     Ch_1_Par_Val == 'A', Ch_2_Par_Val == 'C'    */
    /* third call:      Ch_1_Par_Val == 'B', Ch_2_Par_Val == 'C'    */
{
  Capital_Letter        Ch_1_Loc;
  Capital_Letter        Ch_2_Loc;

  Ch_1_Loc = Ch_1_Par_Val;
  Ch_2_Loc = Ch_1_Loc;
  if (Ch_2_Loc != Ch_2_Par_Val)
    /* then, executed */
    return (Ident_1);
  else  /* not executed */
  {
    Ch_1_Glob = Ch_1_Loc;
    return (Ident_2);
   }
} /* Func_1 */


Boolean Func_2 (Str_30 Str_1_Par_Ref, Str_30 Str_2_Par_Ref)
/*************************************************/
    /* executed once */
    /* Str_1_Par_Ref == "DHRYSTONE PROGRAM, 1'ST STRING" */
    /* Str_2_Par_Ref == "DHRYSTONE PROGRAM, 2'ND STRING" */
{
      One_Thirty        Int_Loc;
      Capital_Letter    Ch_Loc;

  Int_Loc = 2;
  while (Int_Loc <= 2) /* loop body executed once */
    if (Func_1 (Str_1_Par_Ref[Int_Loc],
                Str_2_Par_Ref[Int_Loc+1]) == Ident_1)
      /* then, executed */
    {
      Ch_Loc = 'A';
      Int_Loc += 1;
    } /* if, while */
  if (Ch_Loc >= 'W' && Ch_Loc < 'Z')
    /* then, not executed */
    Int_Loc = 7;
  if (Ch_Loc == 'R')
    /* then, not executed */
    return (true);
  else /* executed */
  {
    //if (strcmp (Str_1_Par_Ref, Str_2_Par_Ref) > 0)
    //if (memcmp (Str_1_Par_Ref, Str_2_Par_Ref, 30) > 0)
    if (Str_1_Par_Ref > Str_2_Par_Ref)
      /* then, not executed */
    {
      Int_Loc += 7;
      Int_Glob = Int_Loc;
      return (true);
    }
    else /* executed */
      return (false);
  } /* if Ch_Loc */
} /* Func_2 */


Boolean Func_3 (Enumeration Enum_Par_Val)
/***************************/
    /* executed once        */
    /* Enum_Par_Val == Ident_3 */
{
  Enumeration Enum_Loc;

  Enum_Loc = Enum_Par_Val;
  if (Enum_Loc == Ident_3)
    /* then, executed */
    return (true);
  else /* not executed */
    return (false);
} /* Func_3 */

version (Win32)
{
    import std.c.windows.windows;

    double dtime()
    {
     double q;

     q = cast(double)GetTickCount() * 1.0e-03;
	    
     return q;
    }
}

version (linux)
{
    import std.c.linux.linux;

    double dtime()
    {
     double q;

     q = cast(double)time(null);
	    
     return q;
    }
}

version (darwin)	// supplied by Anders F Bjorklund
{
    import std.c.darwin.darwin;

    double dtime()
    {
     double q;
     timeval tv;

     gettimeofday(&tv,null);
     q = cast(double)tv.tv_sec + cast(double)tv.tv_usec * 1.0e-6;
     
     return q;
    }
}
