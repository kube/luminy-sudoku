#include "ez-draw.h"
#define HAUT_TITRE 50
#define HAUT_LEGENDE 0
#define BAS_FOOTER 50
#define min(a,b) (a<=b?a:b)

// NOMBRE D'ETAPES MAXIMUM DE L'ALGORITHME
// (PERMET D'EVITER DE BLOQUER LE PROGRAMME)
#define NB_ETAPES_MAXIMUM 900000
#define EM_ACTIF 1

// REPERES GRAPHIQUES CONSTANTS
#define BOUTON_R 18			// Rayon Bouton Retour
#define DECALAGE_OMBRE 1	// Décalage Ombre
#define LARGEUR_MENU 180
#define HAUTEUR_LIGNE_MENU 36

// TAILLE DE FENETRE MINIMUM
#define LARGEUR_FENETRE_MINI 400
#define HAUTEUR_FENETRE_MINI 500

// DIFFICULTES (Nombre de Chiffres total dans Grille)
#define DIFFICULTE_FACILE 45
#define DIFFICULTE_MOYEN 35
#define DIFFICULTE_DIFFICILE 24

enum {false,true};
typedef unsigned char Boolean;
// Les pavés sont numérotés de 0 à 8 ainsi que les lignes et les colonnes.
// Pave[i][j] vrai ssi j est présent dans le pavé n°i.
// l[i][j] vrai ssi j est présent dans la ligne n° i.
// c[i][j] vrai ssi j est présent dans la colonne n° i.
Boolean pave[9][10], l[9][10], c[9][10];

int enonce[9][9]={
				 {7,9,0,0,4,0,0,0,0},
				 {0,0,4,0,1,0,8,7,0},
				 {0,0,0,0,0,2,0,6,0},
				 {0,5,6,0,0,1,0,0,3},
				 {0,0,0,0,5,0,0,0,0},
				 {3,0,0,8,0,0,7,1,0},
				 {0,8,0,2,0,0,0,0,0},
				 {0,3,5,0,8,0,1,0,0},
				 {0,0,0,0,6,0,0,5,8}
				};
int grille[9][9];

Boolean solutione = 0;
Boolean guide=0;

int nbetapes=0;

enum {MODE_SOL,MODE_JEU,MODE_CREATION,MODE_DEMARRAGE};
// Le mode démarage est celui où l'on propose 2 boutons: 
// Soit de jouer une grille proposée.
// Soit de créer une nouvelle grille
int mode=MODE_DEMARRAGE;

// Passe à 1 si le menu est ouvert
Boolean MENU=0;
int CurseurMenu=-1;
int nbelementsmenu=0;

// Boite de Message, passe à 1 si la Boite de Message doit s'afficher
Boolean MESSAGEBOX;
char * MessageContent;

// Boite de sélection de la Difficulté
Boolean SELECTEURDIFFICULTE=0;
int difficulte=25;

// REPERES GRAPHIQUES
int TailleCase=40;
int TailleChiffre=1;
int DecalageBord=0;
int CentreHorizontal=200;
// ZONE DE BOUTONS
int ZoneLargeur=150;
int ZoneX;
int ZoneY=300;

int BoutonLargeur;
int BoutonHauteur=50;
int BoutonsEcart=22;

// COULEURS PERSONNALISEES
unsigned long kube_blue, kube_pink, kube_grey, kube_ombre, kube_ombre2;

// i_clic et j_clic indique les indices ligne et colonne de la case où l'on a cliqué
// -1 indiquant que l'on a cliqué en dehors de la grille
int i_clic=-1, j_clic=-1;

// win fenêtre de jeu, win2 fenêtre d'aide
Window win,win2=None;


/**** ALGORITHMIQUE *************************************************************************/

// Possible renvoie vrai ssi on peut placer k dans la case de coordonnées (i,j)
Boolean Possible (int i, int j, int k)
{
	if(l[i][k] || c[j][k] || pave[(i/3)*3+j/3][k])
		return 0;
	else
		return 1;
}

// Initialise les tableaux de Booléens, le tableau grille et vérifie la cohérence de l'énoncé
// Si jeu=0, affiche Grille vierge.
void Initialisations(Boolean jeu)
{
	int i, j, k;
	
	// On remet les tableaux de booléens à zéro
	for(i=0; i<9; i++)
	for(j=0; j<9; j++)
	for(k=0; k<10; k++)
	{
		grille[i][j]=0;
		pave[(i/3)*3+(j/3)][k]=0;
		c[j][k]=0;
		l[i][k]=0;
		i_clic=-1;
		j_clic=-1;
	}
	
	if(jeu)
	{
		// On met à jour la grille avec l'énoncé
		for(i=0;i<9;i++)
		for(j=0;j<9;j++)
		{
			// Grille
			grille[i][j]=enonce[i][j];
			
			// Booléens
			if(enonce[i][j]!=0)
			{
				pave[(i/3)*3+j/3][enonce[i][j]]=1;
				c[j][enonce[i][j]]=1;
				l[i][enonce[i][j]]=1;
			}
		}
	}
	solutione=0;
	MENU=0;
	nbelementsmenu=0;
}

// Génère une grille au hasard en fonction de l'énoncé, et peut donc résoudre un énoncé quelconque.
void GenereGrille(int prof)
{
	int i, j, k, p;
	int l1, l2, cache, hasard[10];
	
	// Génère des inversions de chiffres
	for(k=1; k<10; k++) hasard[k]=k;
	for(i=0; i<30; i++)
	{
		l1=rand()%9+1;
		l2=rand()%9+1;
		cache=hasard[l1];
		hasard[l1]=hasard[l2];
		hasard[l2]=cache;
	}

		
	// Placer des chiffres dans la Grille
	i = prof/9;
	j = prof%9;
	p=(i/3)*3+(j/3);

	if(prof<81)
	{
		if(grille[i][j] == 0)
		{
			for(k=1; k<10; k++)	
			{
				if(Possible(i, j, hasard[k]))
				{
					l[i][hasard[k]] = c[j][hasard[k]] = pave[p][hasard[k]] = 1;	
					grille[i][j] = hasard[k];
					
					GenereGrille(prof+1);
					

					if(solutione == 0)
					{
						l[i][hasard[k]] = c[j][hasard[k]] = pave[p][hasard[k]] = 0; 
						grille[i][j] = 0;
					}
				}
			}
		}
		else
			GenereGrille(prof+1);
	}
	else
	{
		solutione = 1;
	}
} 

// Renvoie le nombre de solutions à l'énoncé
int CompteSolutions(int prof)
{
	int i, j, k, p, nb;
	
	nb=0;
	nbetapes++;
	
	if(!EM_ACTIF || (nbetapes<=NB_ETAPES_MAXIMUM && EM_ACTIF))
	{
		
		// Placer les chiffres Solution dans la Grille Cache
		i = prof/9;
		j = prof%9;
		p=(i/3)*3+(j/3);

		if(prof<81)
		{
			if(grille[i][j] == 0)
			{
				for(k=1; k<10; k++)	
				{
					if(Possible(i, j, k))
					{
						l[i][k] = c[j][k] = pave[p][k] = 1;	
						grille[i][j] = k;
						
						nb=nb+CompteSolutions(prof+1);
						
						l[i][k] = c[j][k] = pave[p][k] = 0; 
						grille[i][j] = 0;
						
						if(nb>1)
							return nb;					
					}
				}
				if(k==10)return nb;
			}
			else
				nb=nb+CompteSolutions(prof+1);
		}
		else
		{
			nb++;
		}
	}
	return nb;
	
} 

// Epure la Grille complète, pour laisser la grille énoncé correcte et jouable
// en fonction de la difficulté
void EpureGrille(int prof)
{
	int i, j, p, cache;
	nbetapes++;
	
	if(!EM_ACTIF || (nbetapes<=NB_ETAPES_MAXIMUM && EM_ACTIF))
	{
		// Supprimer des chiffres dans la Grille
		i=rand()%9;
		j=rand()%9;	
		p=(i/3)*3+(j/3);

		if(prof<(81-difficulte))
		{
			if(grille[i][j] != 0)
			{
						cache=grille[i][j];
						
						l[i][cache] = c[j][cache] = pave[p][cache] = 0;	
						grille[i][j] = 0;
						
						
						if(CompteSolutions(0)!=1)
						{
							l[i][cache] = c[j][cache] = pave[p][cache] = 1; 
							grille[i][j] = cache;						
							
							EpureGrille(prof);
						}
						else EpureGrille(prof+1);

			}
			else
				EpureGrille(prof);
		}
		else
		{

		}
	}
} 

// Copie tout le contenu de la Grille dans l'énoncé
void PasseGrilleModeJeu()
{
	int i, j;
	for(i=0; i<9; i++)
	for(j=0; j<9; j++)
		enonce[i][j]=grille[i][j];
	
	Initialisations(1);
	mode=MODE_JEU;
}

// Change le chiffre de la case [i][j] avec la valeur k
void ChangeChiffreCase(int i, int j, int k)
{
	// On nettoie le tableau de Booléens si nécessaire
	if(grille[i][j]!=0)
	{
		pave[(i/3)*3+j/3][grille[i][j]]=0;
		l[i][grille[i][j]]=0;
		c[j][grille[i][j]]=0;
	}
	
	// On change la valeur de la grille
	grille[i][j]=k;
	
	// Et on affecte le changement au tableau de Booléens
	pave[(i/3)*3+j/3][k]=1;
	l[i][k]=1;
	c[j][k]=1;
}

// Placer l'entier val dans la case du tableau Grille qui correspond à la case cliquée, penser à
// mettre éventuellement à jour les tableaux de Booléens,  puis redessiner toute la Grille
void AffecterClic (Ez_event *ev, int val)
{
	if(Possible(i_clic, j_clic, val) && i_clic!=-1 && j_clic!=-1)
	{
		ChangeChiffreCase(i_clic, j_clic, val);
	}
}


/**** FONCTIONS AFFICHAGE *******************************************************************/

void AfficheMessage()
{
	int b;
	
	MESSAGEBOX=1;
	
	b=ZoneLargeur*2;
	
	// BOITE
	ez_set_color (kube_ombre2);
	ez_fill_rectangle (win, ZoneX+13, ZoneY-2*BoutonHauteur+13, ZoneX+b+13, ZoneY+BoutonHauteur+13);		
	// BOITE
	ez_set_color (kube_grey);
	ez_fill_rectangle (win, ZoneX, ZoneY-2*BoutonHauteur, ZoneX+b, ZoneY+BoutonHauteur);
	ez_set_color (kube_ombre);
	ez_set_thick(1);
	ez_draw_rectangle (win, ZoneX, ZoneY-2*BoutonHauteur, ZoneX+b, ZoneY+BoutonHauteur);		
		
	ez_set_nfont(3);
	ez_set_color (kube_ombre2);
	ez_draw_text (win, EZ_MC, CentreHorizontal+DECALAGE_OMBRE, ZoneY-20+DECALAGE_OMBRE, MessageContent);
	ez_set_color (ez_white);
	ez_draw_text (win, EZ_MC, CentreHorizontal, ZoneY-20, MessageContent);

}

void DessineGrille(Boolean jeu)	// Dessine la Grille avec les Chiffres, si jeu=0, Grille vierge.
{	
	
	int a, b, b2, c, z, z2, i, j, k;

	// REPERES GRAPHIQUES
	a=HAUT_TITRE+HAUT_LEGENDE;
	b=TailleCase/2+DecalageBord;
	b2=TailleCase/2;
	c=TailleCase;
	z=HAUT_TITRE+HAUT_LEGENDE+c*10;
	z2=z+BAS_FOOTER-20;
	
	// Dessine le Quadrillage de la Grille
	ez_set_color (kube_blue);
	ez_set_thick (1);
	for(i=0; i<=9; i++)
	{
		ez_draw_line (win, b+c*i, a+b2, b+c*i, a+b2+c*9);		 
		ez_draw_line (win, b, a+b2+c*i, b+c*9, a+b2+c*i);
	}
	ez_set_thick (3);
	for(i=0; i<=9; i=i+3)
	{
		ez_draw_line (win, b+c*i, a+b2, b+c*i, a+b2+c*9);		 
		ez_draw_line (win, b, a+b2+c*i, b+c*9, a+b2+c*i);
	}
	
	// Dessine Case Cliquée
	if(i_clic!=-1 && j_clic!=-1)
	{	

		ez_set_thick (3);
		ez_set_color (kube_pink);
		
		// Lignes Horizontales
		ez_draw_line (win, b+c*j_clic, b2+a+i_clic*c, b+c*(j_clic+1), b2+a+i_clic*c);
		ez_draw_line (win, b+c*j_clic, b2+a+(i_clic+1)*c, b+c*(j_clic+1), b2+a+(i_clic+1)*c);

		// Lignes Verticales
		ez_draw_line (win, b+c*j_clic, b2+a+i_clic*c, b+c*j_clic, b2+a+(i_clic+1)*c);
		ez_draw_line (win, b+c*(j_clic+1), b2+a+i_clic*c, b+c*(j_clic+1), b2+a+(i_clic+1)*c);
		
		if(guide)
		{
			// Affiche chiffres jouables
			ez_set_nfont(TailleChiffre-1);
			ez_set_color (kube_ombre);
			if(grille[i_clic][j_clic]==0)
			{
				for(k=1; k<10; k++)
				{
					int positionX, positionY;
					positionX=(c/4)*((k-1)/3+1);
					positionY=(c/4)*((k-1)%3+1);
					
					if(Possible(i_clic, j_clic, k))
					{
						ez_draw_text(win, EZ_MC, b+j_clic*c+positionX, a+i_clic*c+b2+positionY, "%d", k);
					}
				}
			}
		}
	}

	// Affiche les Chiffres en Bleu ceux de l'énoncé et en Rose ceux joués
	ez_set_nfont(TailleChiffre);
	for(i=0;i<9;i++)
	for(j=0;j<9;j++)
	{
		if(enonce[i][j]!=0 && jeu)
		{
			// Affiche Grille Enoncé
			ez_set_color (kube_blue);
			ez_draw_text (win, EZ_MC, b+j*c+b2, a+(i+1)*c, "%d", enonce[i][j]);
		}
		else
		{
			if(grille[i][j]>0)
			{
				// Affiche Chiffres Joués
				ez_set_color (kube_pink);
				ez_draw_text (win, EZ_MC, b+j*c+b2, a+(i+1)*c, "%d", grille[i][j]);				
			}
		}
	}

}

void AfficheLigneMenu(int index, char * texte, char * raccourci)
{
	int a, b, b2, c, z, z2;
	nbelementsmenu=index+1;
	
	// REPERES GRAPHIQUES
	a=HAUT_TITRE+HAUT_LEGENDE;
	b=TailleCase/2+DecalageBord;
	b2=TailleCase/2;
	c=TailleCase;
	z=HAUT_TITRE+HAUT_LEGENDE+c*10;
	z2=z+BAS_FOOTER-20;
	
	// BOUTON
	ez_set_thick(1);
	ez_set_nfont(1);
	ez_set_color(ez_white);
	ez_draw_rectangle(win, b-1, z-HAUTEUR_LIGNE_MENU*index-1, b+LARGEUR_MENU+1, z-HAUTEUR_LIGNE_MENU*index-1);
	
	if(CurseurMenu!=index)
		ez_set_color(kube_ombre);
	else
		ez_set_color(kube_blue);
	ez_fill_rectangle(win, b, z-HAUTEUR_LIGNE_MENU*(index+1), b+LARGEUR_MENU, z-HAUTEUR_LIGNE_MENU*index-1);
	
	ez_set_color(kube_grey);
	ez_draw_line(win, b, z-HAUTEUR_LIGNE_MENU*index-1, b+LARGEUR_MENU, z-HAUTEUR_LIGNE_MENU*index-1);
	
	// OMBRE_TEXTE
	
	if(CurseurMenu!=index)
		ez_set_color(kube_ombre2);
	else
		ez_set_color(ez_black);
	
	ez_draw_text(win, EZ_ML, b+7+DECALAGE_OMBRE, z-HAUTEUR_LIGNE_MENU/2-HAUTEUR_LIGNE_MENU*index-1+DECALAGE_OMBRE, texte);
	ez_draw_text(win, EZ_MR, b+LARGEUR_MENU-7+DECALAGE_OMBRE, z-HAUTEUR_LIGNE_MENU/2-HAUTEUR_LIGNE_MENU*index-1+DECALAGE_OMBRE, raccourci);

	// TEXTE
	ez_set_color(ez_white);
	ez_draw_text(win, EZ_ML, b+7, z-HAUTEUR_LIGNE_MENU/2-HAUTEUR_LIGNE_MENU*index-1, texte);
	ez_draw_text(win, EZ_MR, b+LARGEUR_MENU-7, z-HAUTEUR_LIGNE_MENU/2-HAUTEUR_LIGNE_MENU*index-1, raccourci);

}

void AfficheMenu()
{
	AfficheLigneMenu(0, "Quitter", "Q");
	AfficheLigneMenu(1, "Aide", "F1");
	AfficheLigneMenu(2, "Remise a Zero", "V");
	AfficheLigneMenu(3, "Suppr. Case", "DEL");
	if(mode==MODE_JEU)
		AfficheLigneMenu(4, "Afficher Solution", "S");
	else	
		AfficheLigneMenu(4, "Jouer cette Grille", "J");
	if(guide)
		AfficheLigneMenu(5, "Desactiver Guide", "U");
	else
		AfficheLigneMenu(5, "Activer Guide", "U");
	if(mode==MODE_JEU)
		AfficheLigneMenu(6, "Nouvelle Grille", "");
		
}

void AfficheHeader()
{
	int a, b, b2, c, z, z2;

	// REPERES GRAPHIQUES
	a=HAUT_TITRE+HAUT_LEGENDE;
	b=TailleCase/2+DecalageBord;
	b2=TailleCase/2;
	c=TailleCase;
	z=HAUT_TITRE+HAUT_LEGENDE+c*10;
	z2=z+BAS_FOOTER-20;
	
	// BANDE HAUT
	ez_set_color (kube_grey);
	ez_fill_rectangle (win, 0, 0, CentreHorizontal*2+1, HAUT_TITRE);
	ez_set_color (kube_ombre2);
	ez_draw_line (win, 0, HAUT_TITRE, 2*CentreHorizontal+1, HAUT_TITRE);
	
	// LOGO : OMBRE
	ez_set_nfont(2);
	ez_set_color (kube_ombre);
	ez_draw_text (win, EZ_TC, CentreHorizontal+DECALAGE_OMBRE, 18+DECALAGE_OMBRE, "S U D O K U");
	// LOGO
	ez_set_color (ez_white);
	ez_draw_text (win, EZ_TC, CentreHorizontal, 18, "S U D O K U");


	// BOUTON RETOUR : OMBRE
	ez_set_color (kube_ombre);
	ez_set_thick (4);
	ez_draw_line (win, b+DECALAGE_OMBRE, 16+BOUTON_R/2+DECALAGE_OMBRE, b+BOUTON_R/2+DECALAGE_OMBRE, 16+DECALAGE_OMBRE);
	ez_draw_line (win, b+DECALAGE_OMBRE, 16+BOUTON_R/2+DECALAGE_OMBRE, b+BOUTON_R+DECALAGE_OMBRE, 16+BOUTON_R/2+DECALAGE_OMBRE);
	ez_draw_line (win, b+DECALAGE_OMBRE, 16+BOUTON_R/2+DECALAGE_OMBRE, b+BOUTON_R/2+DECALAGE_OMBRE, 16+BOUTON_R+DECALAGE_OMBRE);
	ez_set_thick (2);	
	ez_draw_circle (win,b-8+DECALAGE_OMBRE,8+DECALAGE_OMBRE,b+BOUTON_R+8+DECALAGE_OMBRE, 16+BOUTON_R+8+DECALAGE_OMBRE); 
	// BOUTON RETOUR	
	ez_set_color (ez_white);
	ez_set_thick (4);
	ez_draw_line (win, b, 16+BOUTON_R/2, b+BOUTON_R/2, 16);
	ez_draw_line (win, b, 16+BOUTON_R/2, b+BOUTON_R, 16+BOUTON_R/2);
	ez_draw_line (win, b, 16+BOUTON_R/2, b+BOUTON_R/2, 16+BOUTON_R);
	ez_set_thick (2);	
	ez_draw_circle (win,b-8,8,b+BOUTON_R+8, 16+BOUTON_R+8); 
}

void AfficheFooter()
{
	int a, b, b2, c, z, z2;

	// REPERES GRAPHIQUES
	a=HAUT_TITRE+HAUT_LEGENDE;
	b=TailleCase/2+DecalageBord;
	b2=TailleCase/2;
	c=TailleCase;
	z=HAUT_TITRE+HAUT_LEGENDE+c*10;
	z2=z+BAS_FOOTER-20;
	
	// On affiche le menu si nécessaire
	if(MENU)
	{
		AfficheMenu();
	}
	
	// Bouton MENU
	if(MENU)
		ez_set_color(kube_ombre2);
	else
		ez_set_color(kube_ombre); 
	ez_fill_rectangle(win, b, z, b+BoutonLargeur, z2);	
	
	// OMBRES TEXTE BOUTONS
	ez_set_nfont(1);
	if(MENU)
		ez_set_color(ez_black);
	else
		ez_set_color(kube_ombre2);
	ez_draw_text (win, EZ_MC, b+BoutonLargeur/2+DECALAGE_OMBRE, (z+z2)/2+DECALAGE_OMBRE, "MENU");
	ez_draw_text (win, EZ_MC, b+BoutonLargeur/2+DECALAGE_OMBRE, (z+z2)/2+DECALAGE_OMBRE, "_   ");
	
	// TEXTE BOUTONS
	ez_set_color(ez_white);
	ez_draw_text(win, EZ_MC, b+BoutonLargeur/2, (z+z2)/2, "MENU");		
	ez_draw_text(win, EZ_MC, b+BoutonLargeur/2, (z+z2)/2, "_   ");
	
	// FLECHE (HAUT OU BAS)
	if(!MENU)
	{
		ez_set_color(kube_ombre2);
		ez_fill_triangle (win, b+BoutonLargeur/2+25+DECALAGE_OMBRE, (z+z2)/2+2+DECALAGE_OMBRE, b+BoutonLargeur/2+29+DECALAGE_OMBRE, (z+z2)/2-2+DECALAGE_OMBRE, b+BoutonLargeur/2+33+DECALAGE_OMBRE, (z+z2)/2+2+DECALAGE_OMBRE);
		ez_set_color(ez_white);
		ez_fill_triangle (win, b+BoutonLargeur/2+25, (z+z2)/2+2, b+BoutonLargeur/2+29, (z+z2)/2-2, b+BoutonLargeur/2+33, (z+z2)/2+2);
	}
	else
	{
		ez_set_color(ez_black);
		ez_fill_triangle (win, b+BoutonLargeur/2+25+DECALAGE_OMBRE, (z+z2)/2-2+DECALAGE_OMBRE, b+BoutonLargeur/2+29+DECALAGE_OMBRE, (z+z2)/2+2+DECALAGE_OMBRE, b+BoutonLargeur/2+33+DECALAGE_OMBRE, (z+z2)/2-2+DECALAGE_OMBRE);
		ez_set_color(kube_ombre);
		ez_fill_triangle (win, b+BoutonLargeur/2+25, (z+z2)/2-2, b+BoutonLargeur/2+29, (z+z2)/2+2, b+BoutonLargeur/2+33, (z+z2)/2-2);
	}
}

void AfficheSelecteurDifficulte()
{		
		int b;
		b=ZoneLargeur*2/3;
		
		ez_set_nfont(2);
		ez_set_color (kube_ombre);
		ez_draw_text (win, EZ_MC, CentreHorizontal+DECALAGE_OMBRE, ZoneY-120+DECALAGE_OMBRE, "S U D O K U");
		ez_set_color (ez_white);
		ez_draw_text (win, EZ_MC, CentreHorizontal, ZoneY-120, "S U D O K U");

		// Texte Sélectionnez votre Niveau :
		ez_set_color (kube_ombre2);
		ez_draw_text (win, EZ_MC, CentreHorizontal, ZoneY-90, "Selectionnez votre niveau :");
		
		// Bordure
		ez_fill_rectangle (win, ZoneX-2, ZoneY-BoutonHauteur-12, ZoneX+b*3+2, ZoneY-8);		
		ez_fill_triangle (win, ZoneX+2*b+b/2-10, ZoneY-10, ZoneX+2*b+b/2, ZoneY, ZoneX+2*b+b/2+10, ZoneY-10);
		
		// Bouton FACILE
		if(CurseurMenu==0)
			ez_set_color (ez_white);
		else
			ez_set_color (kube_grey);
		ez_fill_rectangle (win, ZoneX, ZoneY-BoutonHauteur-10, ZoneX+b, ZoneY-10);		
		
		// Bouton MOYEN
		if(CurseurMenu==1)
			ez_set_color (ez_white);
		else
			ez_set_color (kube_grey);
		ez_fill_rectangle (win, ZoneX+b, ZoneY-BoutonHauteur-10, ZoneX+b*2, ZoneY-10);		

		// Bouton DIFFICILE
		if(CurseurMenu==2)
			ez_set_color (ez_white);
		else
			ez_set_color (kube_grey);
		ez_fill_rectangle (win, ZoneX+2*b, ZoneY-BoutonHauteur-10, ZoneX+b*3, ZoneY-10);
		ez_fill_triangle (win, ZoneX+2*b+b/2-7, ZoneY-10, ZoneX+2*b+b/2, ZoneY-3, ZoneX+2*b+b/2+7, ZoneY-10);		

		// Séparations
		ez_set_color (kube_ombre2);
		ez_draw_line(win, ZoneX+b, ZoneY-BoutonHauteur-10, ZoneX+b, ZoneY-10);
		ez_draw_line(win, ZoneX+b*2, ZoneY-BoutonHauteur-10, ZoneX+b*2, ZoneY-10);
		
		// Textes
		ez_set_nfont(1);
		ez_set_color (kube_ombre2);
		ez_draw_text (win, EZ_MC, ZoneX+b/2, ZoneY-10-BoutonHauteur/2, "FACILE");
		ez_draw_text (win, EZ_MC, ZoneX+b+b/2, ZoneY-10-BoutonHauteur/2, "MOYEN");
		ez_draw_text (win, EZ_MC, ZoneX+b*2+b/2, ZoneY-10-BoutonHauteur/2, "DIFFICILE");
}

void AfficheDemarrage()
{	
		// REPERES GRAPHIQUES
		ZoneX=CentreHorizontal-ZoneLargeur;
		BoutonLargeur=ZoneLargeur-BoutonsEcart/2;


		// BANDE HAUT
		ez_set_color (kube_grey);
		ez_fill_rectangle (win, 0, 0, CentreHorizontal*2+1, ZoneY-30);
		ez_set_color (kube_ombre2);
		ez_draw_line (win, 0, ZoneY-30, 2*CentreHorizontal+1, ZoneY-30);	

		// LOGO : OMBRE
		ez_set_nfont(3);
		ez_set_color (kube_ombre);
		ez_draw_text (win, EZ_TC, CentreHorizontal+DECALAGE_OMBRE, ZoneY-60+DECALAGE_OMBRE, "S U D O K U");
		// LOGO
		ez_set_color (ez_white);
		ez_draw_text (win, EZ_TC, CentreHorizontal, ZoneY-60, "S U D O K U");

		
		// BOUTONS
		//Bouton Créer Grille
		if(CurseurMenu==0 && !SELECTEURDIFFICULTE)
			ez_set_color (kube_ombre2);
		else
			ez_set_color (kube_ombre);
		ez_fill_rectangle (win, ZoneX, ZoneY, ZoneX+BoutonLargeur, ZoneY+BoutonHauteur);		
		//Bouton Jouer Grille
		if(CurseurMenu==1 || SELECTEURDIFFICULTE)
			ez_set_color (kube_ombre2);
		else
			ez_set_color (kube_ombre);
		ez_fill_rectangle (win, ZoneX+BoutonLargeur+BoutonsEcart, ZoneY, ZoneX+2*BoutonLargeur+BoutonsEcart, ZoneY+BoutonHauteur);
		
		if(SELECTEURDIFFICULTE)
			AfficheSelecteurDifficulte();
		
		// OMBRES TEXTE BOUTONS
		ez_set_nfont(1);
		ez_set_color (kube_ombre2);
		
		ez_draw_text (win, EZ_MC, ZoneX+BoutonLargeur+BoutonsEcart+BoutonLargeur/2+DECALAGE_OMBRE, ZoneY+BoutonHauteur/2+DECALAGE_OMBRE, "Jouer Grille");
		ez_set_color (ez_white);
		ez_draw_text (win, EZ_MC, ZoneX+BoutonLargeur+BoutonsEcart+BoutonLargeur/2, ZoneY+BoutonHauteur/2, "Jouer Grille");

		// TEXTE BOUTONS
		ez_set_color (kube_ombre2);
		ez_draw_text (win, EZ_MC, ZoneX+BoutonLargeur/2+DECALAGE_OMBRE, ZoneY+BoutonHauteur/2+DECALAGE_OMBRE, "Creer Grille");
		ez_set_color (ez_white);
		ez_draw_text (win, EZ_MC, ZoneX+BoutonLargeur/2, ZoneY+BoutonHauteur/2, "Creer Grille");
}

void AfficheModeJeu()
{
	// Affiche Bande avec LOGO et Bouton Retour
	AfficheHeader();
	
	//Dessine la Grille Jouée
	DessineGrille(1);

	// Affiche le Footer avec le menu
	AfficheFooter();
	
	if(MESSAGEBOX)AfficheMessage();
}

void AfficheModeCreation()
{
	// Affiche Bande avec LOGO et Bouton Retour
	AfficheHeader();
	
	//Dessine la Grille Jouée
	DessineGrille(0);

	// Affiche le Footer avec le menu
	AfficheFooter();
	
	if(MESSAGEBOX)AfficheMessage();
}


/**** FONCTIONS NAVIGATION ET CLICS *********************************************************/

// Calcule i_clic et j_clic à partir des coordonnées (x,y) si le clic a eu lieu dans la Grille
void ClicCase (int x, int y)
{	
	MESSAGEBOX=0;
	if(!MENU)
	{
		if(mode==MODE_JEU)
		{
			i_clic=(y-(HAUT_LEGENDE+HAUT_TITRE-TailleCase/2))/TailleCase;
			j_clic=(x-(CentreHorizontal-(5*TailleCase+(TailleCase/2))))/TailleCase;
			
			i_clic--;
			j_clic--;
			
			
			if(((i_clic<0) || (i_clic>8)) || ((j_clic<0) || (j_clic>8)) || enonce[i_clic][j_clic]!=0)
			{
					i_clic=-1;
					j_clic=-1;
			}
			else
			{
			}
		}
		else
			if(mode==MODE_CREATION)
			{
				i_clic=(y-(HAUT_LEGENDE+HAUT_TITRE-TailleCase/2))/TailleCase;
				j_clic=(x-(CentreHorizontal-(5*TailleCase+(TailleCase/2))))/TailleCase;
				
				i_clic--;
				j_clic--;
				
				
				if(((i_clic<0) || (i_clic>8)) || ((j_clic<0) || (j_clic>8)))
				{
						i_clic=-1;
						j_clic=-1;
				}
				else
				{
				}
			}
	}
}

// Clic sur les Boutons et Action
void ClicBoutons (int x, int y)
{
	if(mode==MODE_DEMARRAGE)
	{
		//Bouton JOUER
		if(y>=ZoneY && y<=ZoneY+BoutonHauteur && x>=ZoneX+BoutonLargeur+BoutonsEcart && x<=ZoneX+2*BoutonLargeur+BoutonsEcart)
		{
			SELECTEURDIFFICULTE=1;
		}
		//Bouton CREATION
		if(y>=ZoneY && y<=ZoneY+BoutonHauteur && x>=ZoneX && x<=ZoneX+BoutonLargeur)
		{
			mode=MODE_CREATION;
			Initialisations(0);
		}
	}
	
	if(mode==MODE_JEU || mode==MODE_CREATION)
	{
		int b, z, z2;
		b=TailleCase/2+DecalageBord;
		z=HAUT_TITRE+HAUT_LEGENDE+TailleCase*10;
		z2=z+BAS_FOOTER-20;
				
		// Bouton RETOUR
		if(x>=b && x<=b+BOUTON_R && y>=16 && y<=16+BOUTON_R)
		{
			mode=MODE_DEMARRAGE;
			Initialisations(0);
		}
		
		// Bouton MENU
		if(x>=b && x<=b+BoutonLargeur && y>=z && y<=z2) MENU=(1-MENU);

		
	}
}

// Bouger curseur dans Grille et saute les parties de l'énoncé
void DeplaceCurseur(boolean sens, boolean axe) // Sens : 0 moins, 1 plus	Axe : 0 x, 1 y.
{
	int i=0;
	
	if(!MENU)
	{
		do
		{
			if(axe)
			{
				if(sens)
					i_clic=(i_clic+1+9)%9;		// En Bas
				else
					i_clic=(i_clic-1+9)%9;		// En Haut
			}
			else
			{
				if(sens)
					j_clic=(j_clic+1+9)%9;		// A Droite
				else
					j_clic=(j_clic-1+9)%9;		// A Gauche
			}
			i++;
		}
		while(enonce[i_clic][j_clic]!=0 && mode==MODE_JEU && i<10);
	}
	else
		if(MENU)
		{
			if(axe)
			{
				if(sens)
					CurseurMenu=(CurseurMenu-1+nbelementsmenu)%nbelementsmenu;		// En Bas
				else
					CurseurMenu=(CurseurMenu+1+nbelementsmenu)%nbelementsmenu;		// En Haut
			}
		}
	if(mode==MODE_DEMARRAGE)
	{
		if(!SELECTEURDIFFICULTE)
		{
			if(!axe)
			{
				if(sens)
					CurseurMenu=(CurseurMenu-1+2)%2;		// Gauche
				else
					CurseurMenu=(CurseurMenu+1+2)%2;		// Droite
			}
		}
		else
		{
			if(!axe)
			{
				if(sens)
					CurseurMenu=(CurseurMenu+1+3)%3;		// Droite
				else
					CurseurMenu=(CurseurMenu-1+3)%3;		// Gauche
			}
				
		}
	}
}

void win2_event (Ez_event *ev)   // Appellé à chaque événement sur win2, fenêtre d'aide
{
  ez_set_nfont(1);
  switch (ev->type) {

	  case Expose :
		
		ez_set_color (ez_black);
		ez_draw_text (ev->win, EZ_TL, 10, 10,

			"Pour modifier une case, cliquer dessus, puis taper\n"
			"un chiffre entre 1 et 9\n"
			"\n"
			"Vous pouvez utiliser les fleches pour vous deplacer.\n"
			"\n"
			"v     : Pour vider tout. \n"
			"Suppr : Pour vider une case.  \n"
			"s     : Mode solution, pour faire resoudre.\n"
			"q     : Quitter"

		  );
		  break;

	  case ButtonPress :
	  case KeyPress :
		   
	  case WindowClose :
		  ez_window_show (win2, 0);

		  break;
  } 
}

// Traite les diverses possibilités lorsqu'une touche a été pressée
void win1_onKeyPress (Ez_event *ev)
{
	// ON FERME LA BOITE DE MESSAGE
	MESSAGEBOX=0;
	
  switch (ev->key_sym) {

	case XK_Escape	: 	// Appui sur la touche Echap
					if(MENU)
					{
						MENU=0;
						CurseurMenu=-1;
					}
					else
					{
						i_clic=-1;
						j_clic=-1;
					}
					if(SELECTEURDIFFICULTE)
					{
						SELECTEURDIFFICULTE=0;
					}
					break;

	case XK_Return	: 	// Appui sur la touche Entrée
					if(MENU)
					{
						MENU=0;
						switch(CurseurMenu)
						{
							case 0 :
								ez_quit ();
								break;
							case 1 : 
								ez_window_show (win2, 0	);
								ez_window_show (win2, 1);
								break;
							case 2 :
								if(mode==MODE_JEU)
									Initialisations(1);
								else
									Initialisations(0);
								break;
							case 3 :
								ChangeChiffreCase(i_clic, j_clic, 0);
								break;
							case 4 :
								if(mode==MODE_CREATION)
								{	
									
									if(CompteSolutions(0)==1)
									{
										PasseGrilleModeJeu();
										MESSAGEBOX=1;
										MessageContent="GOOD LUCK";
									}
									else
									{
										printf("Grille incorrecte.");
										MESSAGEBOX=1;
										MessageContent="GRILLE INCORRECTE";
									}
								}
								else
								{
									GenereGrille(0);
								}
								break;
							case 5 :
								guide=1-guide;
								break;
							case 6 :
								do
								{
									nbetapes=0;
									Initialisations(0);
									GenereGrille(0);
									EpureGrille(0);
									PasseGrilleModeJeu();
								}while(nbetapes>NB_ETAPES_MAXIMUM);
								MESSAGEBOX=1;
								MessageContent="GOOD LUCK";
								nbetapes=0;
								break;
							default : CurseurMenu=-1;
						}
					}
					else
						if(mode==MODE_DEMARRAGE)
						{
							if(SELECTEURDIFFICULTE)
							{
								switch(CurseurMenu)
								{
									case 0 : difficulte=DIFFICULTE_FACILE; break;
									case 1 : difficulte=DIFFICULTE_MOYEN; break;
									case 2 : difficulte=DIFFICULTE_DIFFICILE;	break;
									default: CurseurMenu=1;
								}
								do
								{
									nbetapes=0;
									Initialisations(0);
									GenereGrille(0);
									EpureGrille(0);
									PasseGrilleModeJeu();
								}while(nbetapes>NB_ETAPES_MAXIMUM);
								mode=MODE_JEU;
								MESSAGEBOX=1;
								MessageContent="GOOD LUCK";
								Initialisations(1);
							}
							else
							{
								switch(CurseurMenu)
								{
									case 0 :
										mode=MODE_CREATION;
										Initialisations(0);
										break;
									case 1 :
										SELECTEURDIFFICULTE=1;
										break;
									default : CurseurMenu=1;
								}
							}
						}
					break;

	case XK_s		: 	// On recherche la solution, puis elle s'affiche
					if(mode==MODE_JEU)
						GenereGrille(0);
					break;
	
	case XK_j		: 	// On passe la grille en mode JEU
					if(mode==MODE_CREATION && CompteSolutions(0)==1)
					{
						PasseGrilleModeJeu();
					}
					break;
					
					
	case XK_k	: 	// On compte le nombre de solutions
					printf("Nombre de Solutions : %d\n\n", CompteSolutions(0));
					break;
					
	case XK_u	: 	// On compte le nombre de solutions
					guide=1-guide;
					break;
					
	case XK_BackSpace	: 	// On retourne au menu principal
					mode=MODE_DEMARRAGE;
					Initialisations(0);
					break;						
					 
	case XK_Up		: 	// On passe à la case du dessus
					DeplaceCurseur(0, 1);
					break;
					 
	case XK_Down	: 	// On passe à la case du dessous
					DeplaceCurseur(1, 1);
					if(SELECTEURDIFFICULTE)
					{
						SELECTEURDIFFICULTE=0;
					}
					break;
					 
	case XK_Left	: 	// On passe à la case de gauche
					DeplaceCurseur(0, 0);
					break;
					 
	case XK_Right	: 	// On passe à la case de droite
					DeplaceCurseur(1, 0);
					break;
					
	case XK_m	: 	// On ouvre le Menu
					MENU=(1-MENU);
					CurseurMenu=-1;
					break;
					

	case XK_Delete:	// On efface le contenu d'une case (on la met à 0)
					// mettre à jour les tableaux de booléens
					ChangeChiffreCase(i_clic, j_clic, 0);
					break;

	case XK_v     :	// On vide toute la grille (i.e on repart de l'énoncé)
					if(mode==MODE_JEU)
						Initialisations(1);
					else
						Initialisations(0);
					break;

	case XK_q	:	ez_quit ();
					break;

	case XK_F1	:	// On affiche la fenêtre d'aide
					ez_window_show (win2, 0	);
					ez_window_show (win2, 1);
					break;

	default		:	// Si un chiffre entre 1 et 9 a été tapée, il faut le placer dans la case cliquÃ©e
					if (ev->key_string[0] >= '1' && ev->key_string[0] <= '9' && !MENU && !MESSAGEBOX)
						AffecterClic (ev, ev->key_string[0] - '0');
  }
}

// Traite les événements qui peuvent se produire dans la fenètre de jeu.
void win_event(Ez_event *ev)
{
   switch (ev->type)
   {
	 case Expose:
		if(mode==MODE_DEMARRAGE)
		{
			AfficheDemarrage();
		}
		if(mode==MODE_JEU)
		{
			AfficheModeJeu();
		}
		if(mode==MODE_CREATION)
		{
			AfficheModeCreation();
		}
		break;
		  
	 case ButtonPress:
			ClicCase(ev->mx,ev->my);
			ClicBoutons(ev->mx,ev->my);
			ez_send_expose(ev->win);
			break;
		  
	 case MotionNotify:
		  break;
		  
	 case ButtonRelease:
		  break;
		  
	 case KeyPress:
		  win1_onKeyPress(ev);
		  ez_send_expose(ev->win);
		  break;
		  
	case ConfigureNotify :  /* On reçoit la nouvelle taille de la fenetre */
	
			// Calcul de la taille de la case en fonction de la nouvelle taille de la fenetre
			TailleCase = (int)min(ev->width/10, (ev->height-HAUT_TITRE-HAUT_LEGENDE-BAS_FOOTER)/10);
			
			// Taille du Chiffre dans la Grille en Fonction de la Taille de la Case
			if(TailleCase<=30)TailleChiffre=0;
			else
				if(TailleCase<=40)TailleChiffre=1;
				else
					if(TailleCase<=50)TailleChiffre=2;
					else TailleChiffre=3;
					
			
			
			// Centre de la fenetre
			CentreHorizontal = ev->width/2;
			
			
			// Décalage de la Grille du bord de la fenetre
			DecalageBord=CentreHorizontal-TailleCase*5;
			
			
			// On redessine la Grille
			ez_send_expose(ev->win);
			
			
			// Si Largeur ou Hauteur trop petite, alors on refixe la taille de la fenêtre au minimum
			if(ev->height<HAUTEUR_FENETRE_MINI){ez_window_set_size(win, ev->width, HAUTEUR_FENETRE_MINI);}
			if(ev->width<LARGEUR_FENETRE_MINI){ez_window_set_size(win, LARGEUR_FENETRE_MINI, ev->height);}
			
			break;
			
	 case WindowClose:
		  ez_quit();
		  break;
   }
}

int main (int nbArg, char * argv[])
{
	if(ez_init()<0) exit(1);

	Initialisations(0);
	
	// COULEURS PERSONNALISEES
	kube_blue=ez_get_RGB (32, 95, 140);
	kube_pink=ez_get_RGB (238, 33, 124);
	kube_grey=ez_get_RGB (210, 207, 199);
	kube_ombre=ez_get_RGB (166, 163, 156);
	kube_ombre2=ez_get_RGB (120, 119, 111);

	win=ez_window_create(TailleCase*10,TailleCase*10+HAUT_TITRE+HAUT_LEGENDE+BAS_FOOTER,"SUDOKU",win_event);
	win2=ez_window_create(450,200,"SUDOKU : AIDE",win2_event);
	ez_window_show (win2, 0	);
	
	ez_window_dbuf (win, 1);
	
	
	// REPERES GRAPHIQUES
	ZoneX=CentreHorizontal-ZoneLargeur;
	BoutonLargeur=ZoneLargeur-BoutonsEcart/2;

	
	
	// REPERES GRAPHIQUES
	ZoneX=CentreHorizontal-ZoneLargeur;
	BoutonLargeur=ZoneLargeur-BoutonsEcart/2;


	// PATIENTER PENDANT CALCUL
	ez_set_color (kube_grey);
	ez_fill_rectangle (win, 0, 0, TailleCase*10, TailleCase*10+HAUT_TITRE+HAUT_LEGENDE+BAS_FOOTER);
	ez_set_nfont(1);
	ez_set_color (kube_ombre);
	ez_draw_text (win, EZ_TC, CentreHorizontal+DECALAGE_OMBRE, ZoneY+10+DECALAGE_OMBRE, "Calcul de la Grille ...");
	ez_draw_text (win, EZ_TC, CentreHorizontal+DECALAGE_OMBRE, ZoneY-40+DECALAGE_OMBRE, "S U D O K U");
	ez_set_color (ez_white);
	ez_draw_text (win, EZ_TC, CentreHorizontal, ZoneY+10, "Calcul de la Grille ...");
	
	printf("\n\nSUDOKU. 2011. Christophe FEIJOO.\n"); 	

	// Si Argument de difficulté passé en paramètre :
	if(nbArg>1)
	{
		if(atoi(argv[1])>=17 && atoi(argv[1])<81)
		{
			difficulte=atoi(argv[1]);
			if(difficulte<22)printf("A VOS RISQUES ET PERILS...\nNe Jetez pas votre ordinateur par la fenetre.\n\nBANZAI!!!!\n");
			do
			{
				nbetapes=0;
				Initialisations(0);
				GenereGrille(0);
				EpureGrille(0);
				PasseGrilleModeJeu();
			}while(nbetapes>NB_ETAPES_MAXIMUM);
		}
		else
		{
			printf("PARAMETRE DE DIFFICULTE INCORRECT.\n");
		}
	}
	
	ez_auto_quit(0);
	ez_main_loop();	
	printf("Aurevoir.\n\n");
	return 0;
}