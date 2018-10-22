#include<iostream>
#include<thread>
#include<mutex>
#include<chrono>
#include<Windows.h>

struct Node
{
	int value;
	int arrivalNumber;
	Node * next;
};

// ===== PROTO FUNC HILOS =====
void barberWaitsClientRunnable(const unsigned int barberId);
void doorWaitsClientRunnable();
void showResultsRunnable();
// ===== FIN PROTO FUNC HILOS =====

// ===== PROTO FUNC BARBERO DORMILON =====
void waitHairCut(Node * client, const unsigned int barberId);
// ===== PROTO FUNC BARBERO DORMILON =====

// ===== FUNC GENERICOS =====
bool mainMenu(int &op);
bool getNumber(int &num);
Node * createNode(const int nodeNumber, const int value);
// ===== FIN FUNC GENERICOS =====

// ===== PROTO FUNC COLA =====
Node * popNode(Node *& head, Node *& tail);
void pushNode(Node *& head, Node *& tail, Node * item);
void showQueue(Node * head);
// ===== FIN PROTO FUNC COLA =====

using namespace std;

// ===== CONSTANTES ===== 
constexpr unsigned int NUMBER_CLIENTS = 10;
constexpr unsigned int NUMBER_CHAIRS = 5;
constexpr unsigned int NUMBER_BARBERS = 3;
// ===== FIN CONSTANTES ===== 

// ===== FIN MIEMBROS GLOBALES HILOS=====

// ===== MIEMBROS GLOBALES HILOS ===== 
bool repeat = true;
Node * head = NULL, * tail = NULL; 
int nodeNumber = 0, inUseChairs = 0;
mutex criticalProcess;
// ===== FIN MIEMBROS GLOBALES HILOS=====


bool mainMenu(int &op) 
{
	cout << "BARBERO DORMILON\n";
	cout << "1.Iniciar\n";
	cout << "0.Salir\n";
	cout << "Eleccion: "; 
	cin >> op;
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(256, '\n');
		return false;
	}
	return true;
}

bool getNumber(int &num)
{
	printf("\nIngrese numero: ");
	cin >> num;
	if (cin.fail() || num < 0)
	{
		cin.clear();
		cin.ignore(256, '\n');
		return false;
	}
	return true;
}

Node * createNode(const int nodeNumber, const int value)
{
	Node * n = new Node();
	n->arrivalNumber = nodeNumber;
	n->next = NULL;
	n->value = value;
	return n;
}

Node * popNode(Node *& head, Node *& tail)
{
	Node * aux = head;
	if (head)
		if (head == tail)
		{
			head = NULL;
			tail = NULL;
		}
		else
			head = head->next;
	return aux;
}

void pushNode(Node *& head, Node *& tail, Node * item)
{
	if (!head)
	{
		head = item;
		tail = item;
	}
	else {
		tail->next = item;
		tail = item;
	}
}

void showQueue(Node * head)
{
	Node * aux = head;
	while (aux)
	{
		cout << "P" <<aux->arrivalNumber << "(" << aux->value << ") ";
		aux = aux->next;
	}
}

void barberWaitsClientRunnable(const unsigned int barberId)
{
	while (repeat) {
		if (head)
		{
			Node * nextClient = popNode(head, tail);
			inUseChairs--;
			waitHairCut(nextClient, barberId);
		}
		else {
			cout << "\n Barbero #" << barberId<< " Durmiendo";
		}
	}
}
void doorWaitsClientRunnable() 
{
	while (repeat && nodeNumber < NUMBER_CLIENTS)
	{		
		int num;
		if (inUseChairs < NUMBER_CHAIRS)
		{
			this_thread::sleep_for(chrono::seconds(rand() % 2 + 1));
			cout << "\nIngresa cliente (Tiempo) " << (num = rand() % 20 + 1);
			Node * item = createNode(++nodeNumber, num);
			pushNode(head, tail, item);
			inUseChairs++;
		}
	}
}

void showResultsRunnable()
{
	showQueue(head);
}

void waitHairCut(Node * client, const unsigned int barberId)
{
	if (client)
	{
		for (int i = 1; i <= client->value; i++)
		{
			cout << "\nBarbero " << barberId << " Cortando pelo del cliente #" << client->arrivalNumber << " Tiempo " << i << " de " << client->value << "/s";
			this_thread::sleep_for(chrono::seconds(1));
		}
	}
}

int main() {
	int op;
	op = 0;
	bool inputError;
	do {
		system("cls");
		//TODO MOSTRAR SILLAS Y BARBEROS PARAMETROS
		if (inputError = !mainMenu(op)) continue;
		switch (op)
		{
		case 1:
		{
			repeat = true;
			

			// ===== MOSTRAR RESULTADOS ===== 
			thread hShowResultsThread(showResultsRunnable);
			// ===== FIN MOSTRAR RESULTADOS =====

			// ===== ESPERAR CLIENTE PUERTA ===== 
			thread hDoorThread(doorWaitsClientRunnable);
			// ===== ESPERAR CLIENTE PUERTA =====

			// ===== ESPERAR BARBERO A CLIENTE  ===== 
			thread hBarberThreads[NUMBER_BARBERS];
			for (int i = 0; i < NUMBER_BARBERS; i++)
				hBarberThreads[i] = thread(barberWaitsClientRunnable, i + 1);
			// ===== ESPERAR PUERTA =====

			// ===== UNIR HILOS AL PROCESO MAIN ===== 
			hShowResultsThread.join();
			for (int i = 0; i < NUMBER_BARBERS; i++)
				hBarberThreads[i].join();
			hDoorThread.join();
			// ===== FIN UNIR HILOS AL PROCESO MAIN ===== 

			// ===== INICIALIZAR BARBEROS ===== 
			// ===== FIN INICIALIZAR BARBEROS =====
		}
			break;
		default:
			cout << "\nOpcion no encontrada\n";
			break;
		}
		cin.ignore(256,'\n');
		getchar();
	} while (op != 0 || inputError);
	exit(0);
}