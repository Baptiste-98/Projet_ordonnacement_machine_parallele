#include <iostream>   
#include "utilities.h" 
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <typeinfo>
#include <thread>
using namespace std;

// Donnée du pb
int const J = 20; //nombre de jobs
int const M = 7; // nombre de machine
int const H = 1000; // Horizon
int const BigM=100000;
int p[J+1][M+1]; // processing time du job j sur la machine m
bool u[J+1][M+1]; // binaire 1 si job j faisable sur la machine m
int s[J+1][J+1][M+1]; // temps de set si le job j est précédé du job i sur m
int aleat = 23910518;

// parametre de l'algo
int taillePopulation = 50;
int Gmax = 200;
float Pc = 0.8;
float Pm = 0.2;


// création de l'aléatoire
std::random_device rd;
    std::mt19937::result_type seed = rd() ^ (
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
                ).count() +
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
                ).count() );

std::mt19937 gen(seed);

    
// création des datas
void creationdata(){
	//creation pij
	for (int i = 1; i <= J; i++){
		for (int j = 1; j <= M; j++){
			std::uniform_int_distribution<int> distrib(1, 14); 
			p[i][j] = distrib(gen);
		}
	}
	// creation u[i][m]	
	std::uniform_int_distribution<int> distribbin(0, 1);
	std::uniform_int_distribution<int> distrib(1, M);	 
	for (int i = 1; i <= J; i++){
		for (int j = 1; j <= M; j++){
			u[i][j]= distribbin(gen);
		} 
		u[i][distrib(gen)]=1;
	}
	// cration sijm
	std::uniform_int_distribution<int> distribs(0, 3);
	for (int i = 1; i <= J; i++){
		for (int j = 1; j <= J; j++){
			for (int k = 1; k <= M; k++){
				if (i==j){
					s[i][j][k] = 0;
				}else{
					s[i][j][k] = distribs(gen);		
				}
			}
		}
	} 	
	
}

// affichage des données au format gusek pour la comparaison des resultats
void affichagedatagusek(){
	//affichage utile pour gusek
	
	cout << "param J := " << J << ";" << endl;
	cout << "param M := " << M << ";" <<endl;
	cout << "param H := " << H << ";" <<endl;
	cout << "param BigM := " << BigM << ";" <<endl;
	
	cout << "param p : " ;
	for (int i = 1; i <= M; i++){
		cout << " " << i << " ";
	}
	cout << ":= ";
	for (int i = 1; i <= J; i++){
		cout << endl;
		cout << "      " << i << "  ";
		for (int j = 1; j <= M; j++){		
		cout << " " << p[i][j] << "  ";
	}
	}
	cout <<";";
	cout << endl;
	cout << "param u : " ;
	for (int i = 1; i <= M; i++){
		cout << " " << i << " ";
	}
	cout << ":= ";
	for (int i = 1; i <= J; i++){
		cout << endl;
		cout << "      " << i << "  ";
		for (int j = 1; j <= M; j++){		
		cout << " " << u[i][j] << "  ";
	}
	}
	cout << ";"<<endl;
	
	
	cout << endl;
	cout << "param s := " ;	
	
	for (int k = 1; k <= M; k++){
		cout << endl;
		cout << " [*,*," <<k<<"] : ";
		for (int i = 1; i <= J; i++){
			cout << i << " ";
		}
		cout<<" := ";
		for (int i = 1; i <= J; i++){
			cout << endl;
			cout << "      " << i << "  ";
			for (int j = 1; j <= J; j++){		
			cout << " " << s[i][j][k] << "  ";
		}
		}
	}
	cout << ";"<<endl;
}

// Création d'un chromosome aléatoire pour la population initial
vector<int> chromosomeAleat(){
	vector<int> chromosome;
	vector<int> jobrest;
	jobrest.clear();
	chromosome.push_back(-4);
	jobrest.push_back(-4);
	std::vector<int>::iterator decaltemp;
	int temp=0;
	//On fait la liste des job qu'il faut encore traiter
	for (int j = 1; j <= J; j++){
		jobrest.push_back(j);	
	} 
	int jobchoisi=0;
	// on choisi le job aleatoirement parmit la liste des job restant et on l'elimine de cette liste
	for (int j = 1; j <= J; j++){
		temp=BigM;
		std::uniform_int_distribution<int> distribj(1,jobrest.size()-1); 
		jobchoisi=jobrest[distribj(gen)];
		jobrest.erase(remove(jobrest.begin(), jobrest.end(), jobchoisi), jobrest.end());
		chromosome.push_back(jobchoisi);
		}
	//cout << endl; 	
	//for (int z = 1; z <= chromosome.size()-1; z++) cout << chromosome[z] << " ";
	//cout << endl; 
	return chromosome;
}

// Calcul de la fonction objective ici  somme des completion time
int sumcompletiontime(vector<int> chromosome){
	int objectif=0;
	int cmachine[M+1];	
	cmachine[0]=BigM;
	// initialise les completion time des machine a 0
	for(int k = 1; k <= M; k++){
		cmachine[k]=0;
	}
	int j = 0;
	int test=0;
	// on parcourt notre chromosome
	for(int i = 1; i <= chromosome.size()-1; i++){
		// Si nous changeons de machine il n'y a rien a faire 
		if (chromosome[i]==-1){
			j=j+1;
			test=1; //car pas de précédence car changement macine
		}else{
			//si c'est la premiere tache de cette machine alores pas de setup time
			if (test==1){
				cmachine[j]=cmachine[j]+p[chromosome[i]][j];
			}else{
				cmachine[j]=cmachine[j]+p[chromosome[i]][j]+s[chromosome[i-1]][chromosome[i]][j];
			}
			test=0;
		}
	}
	for (int k = 1; k <= M; k++){
		objectif=objectif+cmachine[k];
		//cout << " c de la machine " << k << " vaut " << cmachine[k] << endl;
	}
	return objectif;
}

// Croisement des parent en 1 enfant on coupre le chromosome1 en une position tiré aléatoirment et on le sauvegarde jusuqa cette pos dans l'enfant puis on parcours le chromosome 2 et on insert dans l'ordre des job non insérer dans l'enfant.
vector<int> onePointCrossover(vector<int> chromosome1,vector<int> chromosome2){
	vector<int> child;
	vector<int> jobrest;
	jobrest.push_back(-4);
	child.push_back(-4);
	// on initialise les job a inserer dans l'enfant 
	for (int j = 1; j <= J; j++){
		jobrest.push_back(j);	
	} 
	// on tire la position
	std::uniform_int_distribution<int> distribchild(1,J); 
	int pos = distribchild(gen);	
	// on met a jours l'enfant et on efface le job choisi des job restant
	for(int i=1; i<=pos;i++){
		child.push_back(chromosome1[i]);
		jobrest.erase(remove(jobrest.begin(), jobrest.end(), chromosome1[i]), jobrest.end());
	}
	// tant que l'enfant n'a pas traite tous les jobs on parcours le ch2 dans l'ordre et on insert les job manquant
	int i=1;
	while(jobrest.size() != 1){
		if (std::find(jobrest.begin(), jobrest.end(), chromosome2[i]) != jobrest.end()){
			child.push_back(chromosome2[i]);
			jobrest.erase(remove(jobrest.begin(), jobrest.end(), chromosome2[i]), jobrest.end());
		}
		i=i+1;
	}

	return child;
}

// Prend un parent et selectionne aléatoirement 2 genes qui le compose et les inverse simplement 
vector<int> swap(vector<int> chromosome){
	vector<int> child;
	child.push_back(-4);
	// tire au sort la separationd des deux genes
	std::uniform_int_distribution<int> distribchild(1,J); 
	int pos = distribchild(gen);	
	//insert le 2 eme gen a l'enfant 
	for(int i=pos+1; i<=J;i++){
		child.push_back(chromosome[i]);
	}
	//insert le 1er gen a l'enfant 
	for(int i=1; i<=pos;i++){
		child.push_back(chromosome[i]);
	}
	return child;
}

// repartie les machine en fonction de la regle suivante: on affecte les tache a la machine en respectant l'ordre du chromosome et a chaque étape nous calculons les completion time théorique et nous selectionnons le plus petit
vector<int> dispatching_c(vector<int> chromosome){
	// Initialisation des variables
	vector<int> chromosomemachine;
	chromosomemachine.push_back(-4);
	int cmachine[M+1];
	int jobmachine[M+1];
	cmachine[0]=BigM;
	jobmachine[0]=-4;	
	int machinechoisi;
	int tableindice[M+1];
	for (int k = 1; k <= M; k++){
		tableindice[k]=k;
		chromosomemachine.push_back(-1);
		cmachine[k]=0;
		jobmachine[k]=0;
	}	
	tableindice[M+1]=0; 
	int ctemp = INT_MAX;
	// on parcourt tous les jobs 
	for (int i = 1; i <= J; i++){
		ctemp = INT_MAX;
		// on parcourt toute les machines
		for (int k = 1; k <= M; k++){
			// si on a le droit de selectionner cette machine
			if (u[chromosome[i]][k]==1){
				//Si ce n'est pas le premier job qu'on choisi d'affecté a cette machine
				if (jobmachine[k]>=1) {
					// Si le completion time est meilleur avec cette selection alors on le met a jours et on choisi cette machine
					if (ctemp > cmachine[k]+p[chromosome[i]][k]+s[chromosomemachine[tableindice[k]+jobmachine[k]]][chromosome[i]][k]){
						ctemp =  cmachine[k]+p[chromosome[i]][k]+s[chromosomemachine[tableindice[k]+jobmachine[k]]][chromosome[i]][k];
						machinechoisi = k;
					}	
				}
				else{ // idem sans les setup car 1er job sur la machine
					if (ctemp > cmachine[k]+p[chromosome[i]][k]){
						ctemp =  cmachine[k]+p[chromosome[i]][k];
						machinechoisi = k;				
					}
				}	
			}
		}
		//maj du completion time de la machine chosi
		cmachine[machinechoisi]=ctemp;
		//maj du nombre de job sur la machine choisi
		jobmachine[machinechoisi]=jobmachine[machinechoisi]+1;
		//maj du chromosome codifier avec les machines
		chromosomemachine.insert(chromosomemachine.begin()+tableindice[machinechoisi]+jobmachine[machinechoisi],chromosome[i]);
		//mise a jour des indice des positions des machines
		for (int k=machinechoisi+1; k<=M;k++) tableindice[k]=tableindice[k]+1;	
	}	
	return chromosomemachine;
}

// repartie les machine en fonction de la regle suivante: on affecte les tache a la machine en respectant l'ordre du chromosome et a chaque étape nous calculons les completion time théorique et nous selectionnons le plus petit
vector<int> dispatching_p(vector<int> chromosome){
	// Initialisation des variables
	vector<int> chromosomemachine;
	chromosomemachine.push_back(-4);
	int cmachine[M+1];
	int jobmachine[M+1];
	cmachine[0]=BigM;
	jobmachine[0]=-4;
	int machinechoisi;
	int tableindice[M+1];
	for (int k = 1; k <= M; k++){
		tableindice[k]=k;
		chromosomemachine.push_back(-1);
		cmachine[k]=0;
		jobmachine[k]=0;
	}	
	tableindice[M+1]=0; 
	int ptemp = INT_MAX;
	// on parcourt tous les jobs 
	for (int i = 1; i <= J; i++){
		ptemp = INT_MAX;
		// on parcourt toute les machines
		for (int k = 1; k <= M; k++){
			// si on a le droit de selectionner cette machine
			if (u[chromosome[i]][k]==1){
				//Si ce n'est pas le premier job qu'on choisi d'affecté a cette machine
				if (jobmachine[k]>=1) {
					// Si le temps de réalisation est meilleur avec cette selection alors on le met a jours et on choisi cette machine
					if (ptemp > p[chromosome[i]][k]+s[chromosomemachine[tableindice[k]+jobmachine[k]]][chromosome[i]][k]){
						ptemp =  p[chromosome[i]][k]+s[chromosomemachine[tableindice[k]+jobmachine[k]]][chromosome[i]][k];
						machinechoisi = k;
					}	
				}
				else{// idem sans les setup car 1er job sur la machine
					if (ptemp > p[chromosome[i]][k]){
						ptemp =  p[chromosome[i]][k];
						machinechoisi = k;				
					}
				}	
			}
		}
		//maj du completion time de la machine chosi
		cmachine[machinechoisi]=cmachine[machinechoisi]+ptemp;
		//maj du nombre de job sur la machine choisi
		jobmachine[machinechoisi]=jobmachine[machinechoisi]+1;
		//maj du chromosome codifier avec les machines
		chromosomemachine.insert(chromosomemachine.begin()+tableindice[machinechoisi]+jobmachine[machinechoisi],chromosome[i]);
		//mise a jour des indice des positions des machines
		for (int k=machinechoisi+1; k<=M;k++) tableindice[k]=tableindice[k]+1;	
	}	
	return chromosomemachine;
}

// repartie les machine en fonction de la regle suivante: on affecte les tache a la machine en respectant l'ordre du chromosome et a chaque étape nous calculons les completion time théorique et nous selectionnons la premiere machine qui a un completion time <H
vector<int> dispatching_s(vector<int> chromosome){
	// Initialisation des variables
	vector<int> chromosomemachine;
	chromosomemachine.push_back(-4);
	int cmachine[M+1];
	int jobmachine[M+1];
	cmachine[0]=BigM;
	jobmachine[0]=-4;	
	int machinechoisi;
	int tableindice[M+1];
	for (int k = 1; k <= M; k++){
		tableindice[k]=k;
		chromosomemachine.push_back(-1);
		cmachine[k]=0;
		jobmachine[k]=0;
	}	
	tableindice[M+1]=0; 
	int ctemp = INT_MAX;
	// on parcourt tous les jobs 
	for (int i = 1; i <= J; i++){
		ctemp = INT_MAX;
		// on parcourt toute les machines
		for (int k = 1; k <= M; k++){
			// si on a le droit de selectionner cette machine
			if (u[chromosome[i]][k]==1){
				//Si ce n'est pas le premier job qu'on choisi d'affecté a cette machine
				if (jobmachine[k]>=1) {
					// Si le completion time respecte l'horizon avec cette selection alors on le met a jours et on choisi cette machine
					if (cmachine[k]+p[chromosome[i]][k]+s[chromosomemachine[tableindice[k]+jobmachine[k]]][chromosome[i]][k] < H){
						ctemp =  cmachine[k]+p[chromosome[i]][k]+s[chromosomemachine[tableindice[k]+jobmachine[k]]][chromosome[i]][k];
						machinechoisi = k;
						cmachine[machinechoisi];
						break;
					}	
				}
				else{ // idem sans les setup car 1er job sur la machine
					if (cmachine[k]+p[chromosome[i]][k]< H){
						ctemp =  cmachine[k]+p[chromosome[i]][k];
						machinechoisi = k;
						cmachine[machinechoisi];
						break;				
					}
				}	
			}
		}
		//maj du nombre de job sur la machine choisi
		jobmachine[machinechoisi]=jobmachine[machinechoisi]+1;
		//maj du chromosome codifier avec les machines
		chromosomemachine.insert(chromosomemachine.begin()+tableindice[machinechoisi]+jobmachine[machinechoisi],chromosome[i]);
		//mise a jour des indice des positions des machines
		for (int k=machinechoisi+1; k<=M;k++) tableindice[k]=tableindice[k]+1;	
	}	
	return chromosomemachine;
}

// selectionne le chromosome a éliminé avec plus la FO est mauvaise plus il a de chance d'être selectionné
int roulette_wheel_selection_p(vector<vector<int>> population){
	vector<vector<int>> resultatpop;
	resultatpop.push_back({-4});
	// sauvegarder les chromosomes dans un nouveau vecteur
	for (int i=1; i<population.size(); i++){
		resultatpop.push_back({-4});
		for (int j=1; j<population[i].size()-1; j++){
				resultatpop[i].push_back(population[i][j]);
			}
	} 
	// ajout des indices pour retrouver le chromosome a éliminer et la FO afin de pouvoir les triers
	for (int i = 1; i <= population.size()-1; i++){	
		resultatpop[i].push_back(i);	
		resultatpop[i].push_back(sumcompletiontime(dispatching_p(population[i])));
	}
	// trier en fonction des FO
	std::sort(resultatpop.begin()+1, resultatpop.end(),
	          [](const std::vector<int>& a, const std::vector<int>& b) {
	  return a[J+1] < b[J+1];
	});
	
	// calcul fitness total
	int totalfitness=0;
	for (int i=1; i<resultatpop.size(); i++){
			totalfitness=totalfitness+ resultatpop[i][J+1];
	}	
	// aditionner les FO pour pouvoir les selectionner
	int S = 0;
	for (int i=1; i<resultatpop.size(); i++){
			resultatpop[i][J+1]=resultatpop[i][J+1]+S;
			S = resultatpop[i][J+1];
	}	
	// tirage au sort de la valeur selectionner afin de trouver le chromosome a eleiminer
	std::uniform_int_distribution<int> distribroulette(1,S); 
	int tirage=distribroulette(gen);	
	int i=1;
	// parcour les FO afin de trouver laquel correspond
	while (tirage > resultatpop[i][J+1]){
		i=i+1;
	}	
	//sauvegarde de la position du chromosome
	int chromosomeselect = resultatpop[i][J];
	return chromosomeselect;
}

// selectionne le chromosome a éliminé avec plus la FO est mauvaise plus il a de chance d'être selectionné
int roulette_wheel_selection_c(vector<vector<int>> population){
	vector<vector<int>> resultatpop;
	resultatpop.push_back({-4});
	// sauvegarder les chromosomes dans un nouveau vecteur
	for (int i=1; i<population.size(); i++){
		resultatpop.push_back({-4});
		for (int j=1; j<population[i].size()-1; j++){
				resultatpop[i].push_back(population[i][j]);
			}
	} 
	// ajout des indices pour retrouver le chromosome a éliminer et la FO afin de pouvoir les triers
	for (int i = 1; i <= population.size()-1; i++){	
		resultatpop[i].push_back(i);	
		resultatpop[i].push_back(sumcompletiontime(dispatching_c(population[i])));
	}
	// trier en fonction des FO
	std::sort(resultatpop.begin()+1, resultatpop.end(),
	          [](const std::vector<int>& a, const std::vector<int>& b) {
	  return a[J+1] < b[J+1];
	});
	
	// calcul fitness total
	int totalfitness=0;
	for (int i=1; i<resultatpop.size(); i++){
			totalfitness=totalfitness+ resultatpop[i][J+1];
	}	
	// aditionner les FO pour pouvoir les selectionner
	int S = 0;
	for (int i=1; i<resultatpop.size(); i++){
			resultatpop[i][J+1]=resultatpop[i][J+1]+S;
			S = resultatpop[i][J+1];
	}	
	// tirage au sort de la valeur selectionner afin de trouver le chromosome a eleiminer
	std::uniform_int_distribution<int> distribroulette(1,S); 
	int tirage=distribroulette(gen);	
	int i=1;
	// parcour les FO afin de trouver laquel correspond
	while (tirage > resultatpop[i][J+1]){
		i=i+1;
	}	
	//sauvegarde de la position du chromosome
	int chromosomeselect = resultatpop[i][J];
	return chromosomeselect;
}

// selectionne le chromosome a éliminé avec plus la FO est mauvaise plus il a de chance d'être selectionné
int roulette_wheel_selection_s(vector<vector<int>> population){
	vector<vector<int>> resultatpop;
	resultatpop.push_back({-4});
	// sauvegarder les chromosomes dans un nouveau vecteur
	for (int i=1; i<population.size(); i++){
		resultatpop.push_back({-4});
		for (int j=1; j<population[i].size()-1; j++){
				resultatpop[i].push_back(population[i][j]);
			}
	} 
	// ajout des indices pour retrouver le chromosome a éliminer et la FO afin de pouvoir les triers
	for (int i = 1; i <= population.size()-1; i++){	
		resultatpop[i].push_back(i);	
		resultatpop[i].push_back(sumcompletiontime(dispatching_s(population[i])));
	}
	// trier en fonction des FO
	std::sort(resultatpop.begin()+1, resultatpop.end(),
	          [](const std::vector<int>& a, const std::vector<int>& b) {
	  return a[J+1] < b[J+1];
	});
	
	// calcul fitness total
	int totalfitness=0;
	for (int i=1; i<resultatpop.size(); i++){
			totalfitness=totalfitness+ resultatpop[i][J+1];
	}	
	// aditionner les FO pour pouvoir les selectionner
	int S = 0;
	for (int i=1; i<resultatpop.size(); i++){
			resultatpop[i][J+1]=resultatpop[i][J+1]+S;
			S = resultatpop[i][J+1];
	}	
	// tirage au sort de la valeur selectionner afin de trouver le chromosome a eleiminer
	std::uniform_int_distribution<int> distribroulette(1,S); 
	int tirage=distribroulette(gen);	
	int i=1;
	// parcour les FO afin de trouver laquel correspond
	while (tirage > resultatpop[i][J+1]){
		i=i+1;
	}	
	//sauvegarde de la position du chromosome
	int chromosomeselect = resultatpop[i][J];
	return chromosomeselect;
}

// algorithme principale avec la dispatching rules basé sur les completion time
int GA_DR_C(){
	//declaration des variables
	vector<vector<int>> population;
	vector<int> resultatpop;
	vector<int> chromosome1;
	vector<int> chromosome2;
	vector<int> child;
	vector<int> childmachine;
	resultatpop.push_back(-4);
	population.push_back({-4});
	int teta =1;
	int delta =1;
	float proba;
	int meilleurresult=INT_MAX;
	vector<int> meilleursequence;
	int chromosomeselect;


	// cration de la population initial et sauvegarde de la meilleur valeur	
	for (int j = 1; j <= taillePopulation; j++){
		chromosome1 = chromosomeAleat();
		population.push_back(chromosome1);
		resultatpop.push_back(sumcompletiontime(dispatching_c(chromosome1)));
		if (meilleurresult>sumcompletiontime(dispatching_c(chromosome1))){
			meilleurresult=sumcompletiontime(dispatching_c(chromosome1));
			meilleursequence=dispatching_c(chromosome1);
		}
	}
	
	// on realise un nombre de tours de boucle dependant du parmètre 
	while(teta <= Gmax){
		delta = 1;
		// pour toute la taille de la popultaion
		while(delta <= taillePopulation){
			// tirer aleatoirement 2 parent 
			std::uniform_int_distribution<int> distribpop(1,population.size()-1);
			chromosome1=population[distribpop(gen)];
			chromosome2=population[distribpop(gen)];
			//tirer une proba
			std::uniform_int_distribution<int> distribproba(0,100000); 
			// en fonction de la proba on va realiser 0,1 ou 2 mutation et ajouter l'enfant a la population 
			if (proba=float(distribproba(gen))/100000<=Pc){
				child=onePointCrossover(chromosome1,chromosome2);
				population.push_back(child);			
			}
			if (proba=float(distribproba(gen))/100000<=Pm){
				child=swap(chromosome1); // attention pas le bon
				population.push_back(child);						
			}
			childmachine=dispatching_c(child);
			// on verifi si l'enfant possède le meilleur resultat si c'est le cas on sauvegarde ca séquence
			if (sumcompletiontime(childmachine) <meilleurresult){
				meilleurresult=sumcompletiontime(childmachine);
				meilleursequence=childmachine;
			}
			delta = delta+1;
		}
		// on diminue la taille de la population avec la roulette_wheel_selection
		while (population.size()-1>taillePopulation){
			chromosomeselect=roulette_wheel_selection_c(population);
			population.erase(population.begin()+chromosomeselect);
		}
		teta=teta+1;
	}
	// on affiche les resultats
	cout << endl;
	cout << "le meilleur resultat de GA_DR_C est : " <<meilleurresult << endl;
	for (int z = 1; z <= meilleursequence.size()-1; z++) cout << meilleursequence[z] << " ";
	cout << endl; 
	cout << "avec un resultat de GA_DR_C: " <<sumcompletiontime(meilleursequence) << endl;
	return meilleurresult;
}

// algorithme principale avec la dispatching rules basé sur les processing time
int GA_DR_P(){
	//declaration des variables
	vector<vector<int>> population;
	vector<int> resultatpop;
	vector<int> chromosome1;
	vector<int> chromosome2;
	vector<int> child;
	vector<int> childmachine;
	resultatpop.push_back(-4);
	population.push_back({-4});
	int teta =1;
	int delta =1;
	float proba;
	int meilleurresult=INT_MAX;
	vector<int> meilleursequence;
	int chromosomeselect;
	

	// cration de la population initial et sauvegarde de la meilleur valeur	
	for (int j = 1; j <= taillePopulation; j++){
		chromosome1 = chromosomeAleat();
		population.push_back(chromosome1);
		resultatpop.push_back(sumcompletiontime(dispatching_p(chromosome1)));
		if (meilleurresult>sumcompletiontime(dispatching_p(chromosome1))){
			meilleurresult=sumcompletiontime(dispatching_p(chromosome1));
			meilleursequence=dispatching_p(chromosome1);
		}
	}
	
	// on realise un nombre de tours de boucle dependant du parmètre 
	while(teta <= Gmax){
		delta = 1;
		// pour toute la taille de la popultaion
		while(delta <= taillePopulation){
			// tirer aleatoirement 2 parent 
			std::uniform_int_distribution<int> distribpop(1,population.size()-1);
			chromosome1=population[distribpop(gen)];
			chromosome2=population[distribpop(gen)];
			//tirer une proba
			std::uniform_int_distribution<int> distribproba(0,100000); 
			// en fonction de la proba on va realiser 0,1 ou 2 mutation et ajouter l'enfant a la population 
			if (proba=float(distribproba(gen))/100000<=Pc){
				child=onePointCrossover(chromosome1,chromosome2);
				population.push_back(child);			
			}
			if (proba=float(distribproba(gen))/100000<=Pm){
				child=swap(chromosome1); // attention pas le bon
				population.push_back(child);						
			}
			childmachine=dispatching_p(child);
			// on verifi si l'enfant possède le meilleur resultat si c'est le cas on sauvegarde ca séquence
			if (sumcompletiontime(childmachine) <meilleurresult){
				meilleurresult=sumcompletiontime(childmachine);
				meilleursequence=childmachine;
			}
			delta = delta+1;
		}
		// on diminue la taille de la population avec la roulette_wheel_selection
		while (population.size()-1>taillePopulation){
			chromosomeselect=roulette_wheel_selection_p(population);
			population.erase(population.begin()+chromosomeselect);
		}
		teta=teta+1;
	}
	// on affiche les resultats
	cout << endl;
	cout << "le meilleur resultat de GA_DR_P est : " <<meilleurresult << endl;
	for (int z = 1; z <= meilleursequence.size()-1; z++) cout << meilleursequence[z] << " ";
	cout << endl; 
	cout << "avec un resultat de GA_DR_P: " <<sumcompletiontime(meilleursequence) << endl;
	return meilleurresult;
}

// algorithme principale avec la dispatching rules basé sur les sequences
int GA_DR_S(){
	//declaration des variables
	vector<vector<int>> population;
	vector<int> resultatpop;
	vector<int> chromosome1;
	vector<int> chromosome2;
	vector<int> child;
	vector<int> childmachine;
	resultatpop.push_back(-4);
	population.push_back({-4});
	int teta =1;
	int delta =1;
	float proba;
	int meilleurresult=INT_MAX;
	vector<int> meilleursequence;
	int chromosomeselect;


	// cration de la population initial et sauvegarde de la meilleur valeur	
	for (int j = 1; j <= taillePopulation; j++){
		chromosome1 = chromosomeAleat();
		population.push_back(chromosome1);
		resultatpop.push_back(sumcompletiontime(dispatching_s(chromosome1)));
		if (meilleurresult>sumcompletiontime(dispatching_s(chromosome1))){
			meilleurresult=sumcompletiontime(dispatching_s(chromosome1));
			meilleursequence=dispatching_s(chromosome1);
		}
	}
	
	// on realise un nombre de tours de boucle dependant du parmètre 
	while(teta <= Gmax){
		delta = 1;
		// pour toute la taille de la popultaion
		while(delta <= taillePopulation){
			// tirer aleatoirement 2 parent 
			std::uniform_int_distribution<int> distribpop(1,population.size()-1);
			chromosome1=population[distribpop(gen)];
			chromosome2=population[distribpop(gen)];
			//tirer une proba
			std::uniform_int_distribution<int> distribproba(0,100000); 
			// en fonction de la proba on va realiser 0,1 ou 2 mutation et ajouter l'enfant a la population 
			if (proba=float(distribproba(gen))/100000<=Pc){
				child=onePointCrossover(chromosome1,chromosome2);
				population.push_back(child);			
			}
			if (proba=float(distribproba(gen))/100000<=Pm){
				child=swap(chromosome1); // attention pas le bon
				population.push_back(child);						
			}
			childmachine=dispatching_s(child);
			// on verifi si l'enfant possède le meilleur resultat si c'est le cas on sauvegarde ca séquence
			if (sumcompletiontime(childmachine) <meilleurresult){
				meilleurresult=sumcompletiontime(childmachine);
				meilleursequence=childmachine;
			}
			delta = delta+1;
		}
		// on diminue la taille de la population avec la roulette_wheel_selection
		while (population.size()-1>taillePopulation){
			chromosomeselect=roulette_wheel_selection_s(population);
			population.erase(population.begin()+chromosomeselect);
		}
		teta=teta+1;
	}
	// on affiche les resultats
	cout << endl;
	cout << "le meilleur resultat de GA_DR_S est : " <<meilleurresult << endl;
	for (int z = 1; z <= meilleursequence.size()-1; z++) cout << meilleursequence[z] << " ";
	cout << endl; 
	cout << "avec un resultat de GA_DR_S: " <<sumcompletiontime(meilleursequence) << endl;
	return meilleurresult;
}


int main(){
	creationdata();
	affichagedatagusek();
	GA_DR_C();
	GA_DR_P();
	GA_DR_S();
	return 0;
}
