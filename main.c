#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//variabile globale

int ok=0;
char sw[5];
char sw1[6];
char quit[2]="q\n";
int verif=0;
FILE *output;

//variabile globale

//structuri

struct nodechar //nod pentru text
{
    char c;
    struct nodechar *next;
    struct nodechar *prev;
};

struct nodecursor //nod pentru lista/stack pentru poz cursor
{
    struct nodechar *cursor;
    struct nodecursor *next;
};

struct nodestring //nod pentru lista/stack comanda
{
    char comand[20];
    struct nodestring *next;
};

struct nodeint //nod pentru lista/stack dimensiuni text
{
    int dim;
    struct nodeint *next;
};

struct stackpointer // stiva pentru istoricul cursorului
{
    struct nodecursor *top;
};

struct stackcomenzi //stiva pentru istoric comenzi
{
    struct nodestring *top;
};

struct stackdim //stiva pentru istoric dimensiuni text
{
    struct nodeint *top;
};

struct lista //lista cu text
{
    struct nodechar *head, *tail;
};

//structuri

void freespointer(struct stackpointer *stiva) //eliberare noduri stiva de cursori
{
    struct nodecursor *flag, *flag1;

    flag=stiva->top;
    flag1=flag;
    while(flag!=NULL)
    {
        flag=flag->next;
        free(flag1);
        flag1=flag;
    }
}

void freescomenzi(struct stackcomenzi *stiva) //eliberare noduri stiva de comenzi
{
    struct nodestring *flag, *flag1;

    flag=stiva->top;
    flag1=flag;
    while(flag!=NULL)
    {
        flag=flag->next;
        free(flag1);
        flag1=flag;
    }
}

void freesdim(struct stackdim *stiva) //eliberare noduri stiva de dimensiuni
{
    struct nodeint *flag, *flag1;

    flag=stiva->top;
    flag1=flag;
    while(flag!=NULL)
    {
        flag=flag->next;
        free(flag1);
        flag1=flag;
    }
}

void freetext(struct lista *text) //eliberare noduri lista cu text
{
    struct nodechar *flag, *flag1;

    flag=text->head;
    flag1=flag;
    while(flag!=NULL)
    {
        flag=flag->next;
        free(flag1);
        flag1=flag;
    }
}

void add_stackcomanda(char *com, struct stackcomenzi *stivcomanda) //functia care adauga un nod nou in stiva de comenzi
{
    struct nodestring *nou;

    nou=malloc(sizeof(struct nodestring));
    strcpy(nou->comand, com);
    nou->next=stivcomanda->top; //se adauga in fata
    stivcomanda->top=nou; //varful stivei este noul nod
}

void add_stackpointer(struct nodechar *cursor, struct stackpointer *cursoristoric) //functia care adauga un nod nou in stiva de cursori
{
    struct nodecursor *nou;

    nou=malloc(sizeof(struct nodecursor));
    nou->cursor=cursor;
    nou->next=cursoristoric->top;
    cursoristoric->top=nou;
}

void add_stackdim(int n, struct stackdim *stivdim) //functia care adauga un nou nod in stiva de dimensiuni text
{
    struct nodeint *nou;

    nou=malloc(sizeof(struct nodeint));
    nou->dim=n;
    nou->next=stivdim->top;
    stivdim->top=nou;
}

void initlista(struct lista *text) //stergerea listei si initializarea legaturilor head->tail
{
    struct nodechar *flag, *flag1;

    flag=text->head->next;
    while(flag!=text->tail) //stergem toate nodurile mai putin head si tail
    {
        flag1=flag->next;
        free(flag);
        flag=flag1;
    }
    text->head->next=text->tail; //refacem legaturile
    text->tail->prev=text->head;
}

void adauga(struct lista *text, struct nodechar *cursor, char litera) //adauga un nod in lista in fata cursorului
{
    struct nodechar *nou, *flag;

    nou=malloc(sizeof(struct nodechar));
    flag=cursor->prev;
    nou->c=litera;
    nou->next=cursor;
    nou->prev=flag;
    flag->next=nou;
    cursor->prev=nou;
}

void citire(struct lista *text, FILE *input, struct nodechar *cursor, struct stackpointer *cursoristoric, struct stackcomenzi *stivcom, struct stackdim *stivdim, struct lista *textredo) //functie citire text
{
    char line[100];
    int dim=0, i, dimundo=0;
    struct nodechar *nou, *copie, *flag, *flag1;

    add_stackcomanda(sw, stivcom);
    if(verif==1) //in cazul in care avem deja ceva scris in lista textredo
    {
        initlista(textredo);
    }
    if(cursor!=text->tail)
    {
        cursor=cursor->next;
    }
    fgets(line, 100, input); //citim prima linie
    while(strcmp(line, sw)!=0 && strcmp(line, sw1)!=0) //cat timp nu dam de linia "::i" sau "::i "
    {
        dim=strlen(line); //dimensiunea liniei citite
        dimundo=dimundo+dim; //adaugam dimensiunea liniei la dimensiunea textului in total
        for(i=0; i<dim; i++) //adaugam fiecare caracter de pe linie in lista
        {
            nou=malloc(sizeof(struct nodechar));
            nou->c=line[i];
            nou->next=cursor;
            nou->prev=cursor->prev;
            copie=cursor->prev;
            cursor->prev=nou;
            copie->next=nou;
            adauga(textredo, textredo->tail, line[i]); //adaugam si in lista de textredo aceleasi caractere
        }
        fgets(line, 100, input); //luam urmatoarea linie
    }
    if(cursor!=text->tail) //in cazul in care am adaugat text in mijlocul unui rand si stergem ultimul caracter pus (adica \n)
    {
        flag=cursor->prev->prev;
        flag1=flag->next;
        flag->next=cursor;
        cursor->prev=flag;
        free(flag1);
    }
    add_stackpointer(cursor, cursoristoric); //adaugam in stiva de cursori pozitia curenta a cursorului
    add_stackdim(dimundo, stivdim); //adaugam in stiva de dimensiuni dimensiunea textului adaugat
    ok=1; //ok devine 1 adica a terminat de pus noile linii in text/lista
    verif=1; //textredo nu este gol
}

//comenzi functii

void save(struct lista *text) //functia de save
{
    struct nodechar *flag;

    flag=text->head->next; //luam primul nod
    output=fopen("editor.out", "w"); //deschidem fisierul
    while(flag!=text->tail) //adaugam toate caracterele din nod in fisier
    {
        fputc(flag->c, output);
        flag=flag->next;
    }
    fclose(output);
}

void backspace(struct lista *text, struct stackpointer *cursoristoric) //functia de backspace
{
    struct nodechar *flag, *flag1, *cursor;

    cursor=cursoristoric->top->cursor; //initializam cursorul cu pozitia lui curenta
    if(cursor!=text->tail) //cazul in care nu ne aflam la capatul textului si stergem caracterul de pe cursor
    {
        flag=cursor->prev;
        flag1=cursor->next;
        flag->next=flag1;
        flag1->prev=flag;
        free(cursor);
        add_stackpointer(flag, cursoristoric); //adaugam noua pozitie a cursorului in stiva
    }
    else //cazul in care ne aflam la capat si stergem caracterul de dinaintea cursorului
    {
        flag=cursor->prev;
        flag1=flag->prev;
        flag1->next=cursor;
        cursor->prev=flag1;
        free(flag);
        add_stackpointer(flag1, cursoristoric); //adaugam noua pozitie a cursorului in stiva
    }
}

void gtline(struct lista *text, struct stackpointer *cursoristoric, char *laux) //functia gl
{
    int dim, nrlinie=0, i, aux, enter=0;
    struct nodechar *flag;

    dim=strlen(laux);
    for(i=0; i<dim-1; i++) //scoatem nr din string-ul de parametrii
    {
        aux=laux[i]-'0';
        nrlinie=nrlinie*10+aux;
    }
    flag=text->head->next; //ne ducem la inceputul textului
    while(enter<nrlinie-1)
    {
        if(flag->c=='\n') //daca gaseste '\n' inseamna ca s-a terminat o linie si trecem la urm linie
        {
            enter++;
        }
        flag=flag->next;
    }
    add_stackpointer(flag, cursoristoric); //adaugam noua pozitie a cursorului in stiva
}

void gcline(struct lista *text, struct stackpointer *cursoristoric, char *laux) //functia gc
{
    int  nrlinie=0, nrcaracter=0, i=0, auxlinie, auxcaracter, enter=0;
    struct nodechar *flag;

    while(laux[i]!='\n' && laux[i]!=' ') //scoatem din string-ul de parametri primul numar aka nr caracterului de pe linie
    {
        auxcaracter=laux[i]-'0';
        nrcaracter=nrcaracter*10+auxcaracter;
        i++;
    }
    if(laux[i]=='\n') //in cazul in care nu mai exista alt numar (parametrul optional)
    {
        nrlinie=-1; //valoare imposibila
    }
    else //in cazul in care avem parametrul al doilea
    {
        i++;
        while(laux[i]!='\n') //scoatem fiecare cifra si o adaugam la nr liniei
        {
            auxlinie=laux[i]-'0';
            nrlinie=nrlinie*10+auxlinie;
            i++;
        }
    }
    if(nrlinie==-1) //cazul in care nu avem parametri
    {
        flag=cursoristoric->top->cursor; //initializam flag cu pozitia cursorului 
        while(flag->c!='\n' && flag!=text->head) //ne ducem la inceputul liniei
        {
            flag=flag->prev;
        }

        flag=flag->next;
        auxcaracter=1;
        while(auxcaracter<nrcaracter) //numaram nr de caractere si flag ajunge pe caracterul dorit
        {
            flag=flag->next;
            auxcaracter++;
        }
    }
    else //cazul in care avem parametrul de nr liniei
    {
        flag=text->head->next; //initializam flagul cu prima pozitie din text
        while(enter<nrlinie-1) //numaram linia (exact codul de la gl)
        {
            if(flag->c=='\n')
            {
                enter++;
            }

            flag=flag->next;
        }
        auxcaracter=1;
        while(auxcaracter<nrcaracter) //nr caracterele si mutam flag
        {
            flag=flag->next;
            auxcaracter++;
        }
    }
    add_stackpointer(flag, cursoristoric); //adaugam noua pozitie a cursorului in stiva
}

void dl(struct lista *text, struct stackpointer *cursoristoric, char *laux) //functia dl
{
    struct nodechar *flag, *flag1, *aux, *aux1;
    int auxlinie, nrlinie=0, i=0, enter=0;

    if(laux[0]=='\n') //cazul in care nu avem parametrul pentru nr liniei
    {
        aux=cursoristoric->top->cursor; //luam pozitia cursorului
        while(aux->c!='\n' && aux!=text->head) //ne ducem la capatul liniei
        {
            aux=aux->prev;
        }
        flag=aux->next;
        while(flag->c!='\n') //cat timp nu trecem pe cealalta linie eliminam toate nodurile/caracterele de pe linie
        {
            flag1=flag;
            flag=flag->next;
            free(flag1);
        }
        aux1=flag->next; //eliminam si nodul "\n"
        free(flag);
        aux->next=aux1;
        aux1->prev=aux;
        add_stackpointer(aux1, cursoristoric); //adaugam noua pozitie a cursorului in stiva
    }
    else //cazul in care avem parametrul pt nr liniei
    {
        laux=laux+1;
        while(laux[i]!='\n') //luam nr liniei din string
        {
            auxlinie=laux[i]-'0';
            nrlinie=nrlinie*10+auxlinie;
            i++;
        }
        flag=text->head->next; //incepem din nou de la capatul liniei
        while(enter<nrlinie-1) //codul de la gl
        {
            if(flag->c=='\n')
            {
                enter++;
            }
            flag=flag->next;
        }
        aux=flag->prev;
        while(flag->c!='\n') //stergem elementele de pe linia respectiva
        {
            flag1=flag;
            flag=flag->next;
            free(flag1);
        }

        aux1=flag->next; //eliminam si "\n"
        free(flag);
        aux->next=aux1; //unim capetele
        aux1->prev=aux;
        add_stackpointer(aux1, cursoristoric); //adaugam noua pozitie a cursorului in stiva
    }
}

void d(struct lista *text, struct stackpointer *cursoristoric, char *laux) //functia d
{
    int nrcaractere=0, i=0, aux, ok=0;
    struct nodechar *flag, *flagp, *del;

    if(laux[0]=='\n') //in cazul in care nu avem specificat parametrul
    {
        nrcaractere=1;
    }
    else //cazul in care avem parametrul optional
    {
        laux=laux+1;
        while(laux[i]!='\n') //scoatem nr din string
        {
            aux=laux[i]-'0';
            nrcaractere=nrcaractere*10+aux;
            i++;
        }
    }
    flag=cursoristoric->top->cursor; //luam pozitia cursorului
    while(ok<nrcaractere) //stergem nrcaractere noduri din text 
    {
        del=flag->next;
        flagp=flag->next->next;
        flag->next=flagp;
        flagp->prev=flag;
        free(del);
        ok++;
    }
    add_stackpointer(flag, cursoristoric); //adaugam noua pozitie a cursorului in stiva
}

void dw(struct lista *text, struct stackpointer *cursoristoric, char *laux) //functia dw
{
    int dim, gasit=0;
    char *cuv;
    struct nodechar *flag, *inc, *incc, *auxc;

    dim=strlen(laux);
    cuv=malloc(sizeof(char)*dim);
    strncpy(cuv, laux, dim-1); //luam cuvantul de sters din string 
    cuv[dim-1]='\0';
    flag=cursoristoric->top->cursor; //luam pozitia cursorului
    if(flag==text->tail) //cazul in care ne aflam la capatul textului si nu mai avem ce sa mai stergem
    {
        add_stackpointer(flag, cursoristoric); //adauga pozitia cursorului
        free(cuv);
    }
    else //cazul in care nu ne aflam la capat
    {
        while(gasit<dim-1 && flag!=text->tail) //cat timp nu am ajuns la capat si nu am gasit cuvantul
        {
            if(flag->c==cuv[gasit]) //verificam daca avem caracterele in ordine ca sa formeze cuvantul cautat
            {
                if(gasit==0)
                {
                    inc=flag; //salvam adresa primului nod din cuvant
                }
                gasit++;
            }
            else //in cazul in care nu am gasit cuvantul si trebuie sa o luam de la capat cu cautarea
            {
                gasit=0;
            }
            flag=flag->next;
        }
        if(flag==text->tail && gasit<dim-1) //cazul in care am ajuns la capat
        {
            add_stackpointer(cursoristoric->top->cursor, cursoristoric); //adaugam pozitia cursorului in stiva
        }
        else //cazul in care am gasit cuvantul
        {
            incc=inc->prev;
            while(inc!=flag) //stergem nodurile care contin caracterele ce formeaza cuvantul cautat
            {
                auxc=inc->next;
                free(inc);
                inc=auxc;
            }
            incc->next=flag; //refacem legaturile dintre nodurile ramase
            flag->prev=incc;
            add_stackpointer(flag, cursoristoric); //adaugam noua poz a cursorului in stiva
            free(cuv);
        }
    }
}

struct nodechar* dwmod(struct lista *text, struct nodechar *cursor ,char *laux) //functia dw modificata; este identica cu functia dw, cu diferenta ca returneaza adresa nodului de dupa stergere
{
    int dim, gasit=0;
    char *cuv;
    struct nodechar *flag, *inc, *incc, *auxc;

    dim=strlen(laux);
    cuv=malloc(sizeof(char)*dim);
    strncpy(cuv, laux, dim-1);
    cuv[dim-1]='\0';
    flag=cursor;
    while(gasit<dim-1 && flag!=text->tail)
    {
        if(flag->c==cuv[gasit])
        {
            if(gasit==0)
            {
                inc=flag;
            }
            gasit++;
        }
        else
        {
            gasit=0;
        }

        flag=flag->next;
    }
    if(flag==text->tail)
    {
        free(cuv);
        return flag;
    }
    else
    {
        incc=inc->prev;
        while(inc!=flag)
        {
            auxc=inc->next;
            free(inc);
            inc=auxc;
        }
        incc->next=flag;
        flag->prev=incc;
        free(cuv);
        return flag;
    }
}

void da(struct lista *text, struct stackpointer *cursoristoric, char *laux) //functia da
{
    struct nodechar *flag, *aux, *verif2, *verif1, *trash;

    flag=cursoristoric->top->cursor;
    aux=flag;
    while(flag!=text->tail) //sterge toate aparitiile cuvantului de dupa cursor
    {
        flag=dwmod(text, flag, laux);
    }
    verif2=text->head->next;
    while(verif2!=text->tail) //verifica posibilitatea ca cumva la stergeri sa nu ramana dublu-\n din cauza ca e posibil sa se stearga o linie intreaga
    {
        if(verif2->next!=text->tail && verif2->c=='\n' && verif2->next->c=='\n') //cazul in care gaseste dublu \n
        {
            verif1=verif2->next->next;
            trash=verif2->next;
            free(trash);
            verif2->next=verif1;
            verif1->prev=verif2;
        }
        verif2=verif2->next;
    }
    add_stackpointer(aux, cursoristoric); //adaugam noua pozitie a cursorului
}

void re(struct lista *text, struct stackpointer *cursoristoric, char *laux) //functia re
{
    struct nodechar *flag, *cursor;
    char oldw[30], neww[30];
    int i=0, aux1=0, dim, ok=0, contor=0;

    while(laux[i]!=' ') //scoatem din string primul cuvant aka cuvantul care trebuie sters
    {
        oldw[aux1]=laux[i];
        aux1++;
        i++;
    }

    oldw[aux1]='\n';
    aux1++;
    oldw[aux1]='\0';
    i++;
    aux1=0;
    while(laux[i]!='\n') //scoatem al doilea cuvant din string aka cuvantul care trebuie adaugat
    {
        neww[aux1]=laux[i];
        i++;
        aux1++;
    }

    neww[aux1]='\0';
    dim=strlen(neww);
    cursor=cursoristoric->top->cursor;
    flag=dwmod(text, cursor, oldw); //sterge prima aparitie de dupa cursor a lui oldw si returneaza adresa nodului de dupa stergere
    if(flag==text->tail) //cazul in care nu a gasit nimic de sters
    {
        return; //functia se opreste
    }
    else //cazul in care a sters un cuvant
    {
        while(ok<dim)
        {
            adauga(text, flag, neww[ok]); //adaugam nodurile cu caracterele cuvantului nou
            if(ok==0 && contor==0) //cursorul trebuie dupa primul nod adaugat sa aiba adresa acestuia
            {
                cursor=flag->prev;
                contor=1;
            }
            ok++;
        }
    }
    add_stackpointer(cursor, cursoristoric); //adaugam noua pozitie a cursorului
}

void ra(struct lista *text, struct stackpointer *cursoristoric, char *laux) //functia ra
{
    struct nodechar *flag, *cursor, *copie;
    char oldw[30], neww[30];
    int i=0, aux1=0, dim, ok=0, contor=0;

    while(laux[i]!=' ') //cuvantul vechi
    {
        oldw[aux1]=laux[i];
        aux1++;
        i++;
    }
    oldw[aux1]='\n';
    aux1++;
    oldw[aux1]='\0';
    i++;
    aux1=0;
    while(laux[i]!='\n') //cuvantul nou
    {
        neww[aux1]=laux[i];
        i++;
        aux1++;
    }
    neww[aux1]='\0';
    dim=strlen(neww);
    cursor=cursoristoric->top->cursor; //luam pozitia cursorului
    flag=cursor;
    while(flag!=text->tail) //cat timp nu suntem la capatul textului
    {
        copie=dwmod(text, flag, oldw); //stergem aparitia cuvantului vechi
        ok=0;
        if(copie==text->tail) //cazul in care nu exista oldw
        {
            break;
        }
        else //cazul in care exista si s-a sters
        {
            while(ok<dim)
            {
                adauga(text, copie, neww[ok]); //adaugam noile noduri cu caracterele din neww

                if(ok==0 && contor==0)
                {
                    cursor=copie->prev; //pozitia cursorului este fixata
                    contor=1;
                }
                ok++;
            }
        }
        flag=copie;
    }
    add_stackpointer(cursor, cursoristoric); //adaugam noua pozitie a cursorului in stiva
}

int pop_stackint(struct stackdim *stiva) //functie care scoate un element din stiva cu dimensiuni
{
    int val;
    struct nodeint *flag;

    val=stiva->top->dim;
    flag=stiva->top;
    stiva->top=stiva->top->next;
    free(flag);
    return val;
}

void undo(struct lista *text, struct stackpointer *cursoristoric, struct stackdim *stivdim, struct stackcomenzi *stivcom) //functia undo (numai pt implementari text)
{

    struct nodechar *flag, *flag1, *flag2;
    int nr=0, ok=0;

    if(strcmp(stivcom->top->next->comand, sw)==0 || strcmp(stivcom->top->next->comand, sw1)==0) //cazul in care facem undo la implementari
    {
        flag=cursoristoric->top->cursor; //luam poz cursorului
        nr=pop_stackint(stivdim); //luam dimensiunea textului adaugat
        while(ok<nr) //stergem nodurile adaugate
        {
            flag1=flag->prev->prev;
            flag2=flag->prev;
            flag1->next=flag;
            flag->prev=flag1;
            free(flag2);
            ok++;
        }
        add_stackpointer(flag, cursoristoric); //adaugam in stiva noua pozitie a cursorului
    }
    else //cazul in care se face undo la alte functii
    {
        return; //teoretic aici ar fi venit undo pentru restul functiilor
    }
}

void redo(struct lista *text, struct lista *textredo, struct stackcomenzi *stivcom,  struct stackpointer *cursoristoric) //functie redo
{
    struct nodechar *flag, *stg;

    if(strcmp(stivcom->top->next->next->comand, sw)==0 || strcmp(stivcom->top->next->next->comand, sw1)==0) //cazul in care facem redo pentru implementare text
    {
        flag=cursoristoric->top->cursor; //luam pozitia cursorului
        stg=textredo->head->next; //incepem de la primul nod din lista de textredo
        while(stg!=textredo->tail) //adaugam nodurile din textredo in lista text
        {
            adauga(text, flag, stg->c);
            stg=stg->next;
        }
    }
    else //cazul in care avem redo pentru celelalte functii
    {
        return; //redo pentru restul functiilor
    }
}

//comenzi functii

int comanda(struct lista *text, struct stackcomenzi *stivcom, FILE *input, struct stackpointer *cursoristoric, struct stackdim *stivdim, struct lista *textredo) //citire comenzi si redirectionare
{
    char line[100], laux[10]; //in laux pastram parametrii de la fiecare comanda

    fgets(line, 100, input);
    while(strcmp(line, sw)!=0 && strcmp(line, sw1)!=0) //cat timp nu dam de "::i"
    {
        add_stackcomanda(line, stivcom); //adaugam "::i" in stiva de comenzi ca sa stim ca am adaugat text
        if(strcmp(line, quit)==0) //daca comanda este q
        {
            return -1;
        }
        else
        {
            if(strcmp(line, "s\n")==0) //save
            {
                save(text);
            }
            if(strcmp(line, "b\n")==0) //backspace
            {
                backspace(text, cursoristoric);
            }
            if(strcmp(line, "u\n")==0) //undo
            {
                undo(text, cursoristoric, stivdim, stivcom);
            }
            if(strcmp(line, "r\n")==0) //redo
            {
                redo(text, textredo, stivcom, cursoristoric);
            }
            if(strstr(line, "gl ")!=NULL) //go to line
            {
                strcpy(laux, line+3);
                gtline(text, cursoristoric, laux);
            }
            if(strstr(line, "gc ")!=NULL) //go to character
            {
                strcpy(laux, line+3);
                gcline(text, cursoristoric, laux);
            }
            if(strstr(line, "dl")!=NULL) //delete line
            {
                strcpy(laux, line+2);
                dl(text, cursoristoric, laux);
            }
            if(line[0]=='d' && (line[1]==' ' || line[1]=='\n')) //delete characters
            {
                strcpy(laux, line+1);
                d(text, cursoristoric, laux);
            }
            if(strstr(line, "dw ")!=NULL) //delete word
            {
                strcpy(laux, line+3);
                dw(text, cursoristoric, laux);
            }
            if(strstr(line, "da ")!=NULL) //delete all word
            {
                strcpy(laux, line+3);
                da(text, cursoristoric, laux);
            }
            if(strstr(line, "re ")!=NULL && line[0]=='r' && line[1]=='e') //replace word
            {
                strcpy(laux, line+3);
                re(text, cursoristoric, laux);
            }
            if(strstr(line, "ra ")!=NULL) //replace all word
            {
                strcpy(laux, line+3);
                ra(text, cursoristoric, laux);
            }
            fgets(line, 100, input); //luam urmatoarea linie
        }
    }
    return 0;
}

int main()
{
    struct lista *text, *textredo;
    struct stackcomenzi *stivcom;
    struct stackpointer *cursoristoric;
    struct stackdim *stivdim;
    struct nodechar *cursor;
    int kill;
    FILE *input;

    //initieri

    text=malloc(sizeof(struct lista));
    text->head=malloc(sizeof(struct nodechar));
    text->tail=malloc(sizeof(struct nodechar));
    text->head->next=text->tail;
    text->head->prev=NULL;
    text->tail->next=NULL;
    text->tail->prev=text->head;
    textredo=malloc(sizeof(struct lista));
    textredo->head=malloc(sizeof(struct nodechar));
    textredo->tail=malloc(sizeof(struct nodechar));
    textredo->head->next=textredo->tail;
    textredo->head->prev=NULL;
    textredo->tail->next=NULL;
    textredo->tail->prev=textredo->head;
    cursor=text->tail;
    stivcom=malloc(sizeof(struct stackcomenzi));
    stivcom->top=NULL;
    cursoristoric=malloc(sizeof(struct stackpointer));
    cursoristoric->top=NULL;
    stivdim=malloc(sizeof(struct stackdim));
    stivdim->top=NULL;
    input=fopen("editor.in", "r");

    //initieri

    strcpy(sw, "::i\n"); //punem in sw ::i
    strcpy(sw1, "::i \n"); //punem in sw ::i cu spatiu
    while(stivcom->top==NULL || strcmp(stivcom->top->comand, quit)!=0) //cat timp nu dam de comanda q sau nu avem comenzi in stiva de comenzi
    {
        if(ok==0) //daca suntem la pasul de citire text
        {
            if(verif!=0) //cazul in care avem cursorul altundeva in afara de pozitia de final trebuie sa initializam adresa cursorului
            {
                cursor=cursoristoric->top->cursor;
            }
            citire(text, input, cursor, cursoristoric, stivcom, stivdim, textredo);
        }
        if(ok==1) //daca suntem la pasul de efectuare a comenzilor
        {
            kill=comanda(text, stivcom, input, cursoristoric, stivdim, textredo);
            if(kill==-1) //daca am ajuns la comanda q
            {
                break;
            }
            else
            {
                ok=0; //trecem inapoi la pasul de a citi text
            }
        }
    }
    fclose(input); //inchidem editor.in

    //eliberari de memorie

    freespointer(cursoristoric);
    free(cursoristoric);
    freescomenzi(stivcom);
    free(stivcom);
    freesdim(stivdim);
    free(stivdim);
    freetext(text);
    free(text);
    freetext(textredo);
    free(textredo);

    //eliberari de memorie

    return 0;
}

