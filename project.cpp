// include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

// pragma comments
#pragma warning(disable:4996)

// constants
int const kMaxCountryName = 21; // max length of a country's name string
int const kMaxParcelString = 60; // max length of a parcel's string (destination, weight, & value together) *this gives extra space just in case
int const kHashTableSize = 127; // number of buckets in the parcels hash table

int const kMinParcelWeight = 100;
int const kMaxParcelWeight = 50000;
float const kMinParcelValue = 10.00;
float const kMaxParcelValue = 2000.00;

// data types
typedef struct Parcel // represents a node in the tree structure
{
	char* destination; 
	int weight; // 100 to 5000 gms
	float valuation; // $10 to $2000

	struct Parcel* rightChild;
	struct Parcel* leftChild;
} Parcel;

typedef struct ParcelHashTable // holds the root nodes of the tree structures
{
	Parcel* table[kHashTableSize];
} ParcelHashTable;

// prototypes
ParcelHashTable* initializeHashTable(void);
Parcel* initializeParcel(char* destination, int weight, float valuation);
unsigned long generateHash(char* str);
void insertIntoTable(ParcelHashTable* hashTable, char* destination, int weight, float valuation);
Parcel* insertSorted(Parcel* parent, char* destination, int weight, float valuation);
Parcel* whichBucketToSearch(ParcelHashTable* hashTable, char* str);
Parcel* getCountry(ParcelHashTable* hashTable);
void parsePreOrder(Parcel* root, int* parcelsPrinted, int* totalWeight, float* totalValue, Parcel** cheapestParcel, Parcel** mostExpensiveParcel, Parcel** lightestParcel, Parcel** heaviestParcel, bool printNodes, int heavier, int amount);



int main(void)
{
	//---------------------------------------------------------------- PARSING & INSERTING ------------------------------------------------------------------------
	/* initializations */
	FILE* fp = NULL;
	ParcelHashTable* myHashTable = initializeHashTable(); // initialize the parcels' hash table

	/* open file for read mode and check for errors */
	if ((fp = fopen("couriers.txt", "r")) == NULL)
	{
		printf("Unable to open file for reading\n");
		return -1;
	}

	char tempParcel[kMaxParcelString] = ""; // temporarily store lines being read from file

	/* parse all parcels from file and insert them into their appropriate BST (bucket) */
	while (fgets(tempParcel, sizeof(tempParcel), fp) != NULL)
	{
		/* skip blank lines */
		if (tempParcel[0] == '\n')
		{
			tempParcel[strlen(tempParcel) - 1] = '\0';
			continue;
		}
		/* remove newline character (\n) */
		if ((strlen(tempParcel) > 0) && (strchr(tempParcel, '\n') != NULL))
		{
			tempParcel[strlen(tempParcel) - 1] = '\0';
		}

		/* temporary storage for parcel data members */
		char tempDestination[kMaxCountryName] = "";
		int tempWeight = 0;
		float tempValuation = 0.00;

		/* extract destination, weight, and valuation data members & check for errors */
		if (sscanf(tempParcel, "%[^,], %d, %f", tempDestination, &tempWeight, &tempValuation) != 3)
		{
			continue; // skip incorrect line but keep running program
		}
		/* checking for destination, weight, and valuation validity */
		if ((tempDestination[kMaxCountryName - 1] != '\0') ||
			(tempWeight < kMinParcelWeight || tempWeight >  kMaxParcelWeight) ||
			(tempValuation < kMinParcelValue || tempValuation >  kMaxParcelValue))
		{
			continue;
		}

		insertIntoTable(myHashTable, tempDestination, tempWeight, tempValuation); // insert parcel into a bucket in the hash table
	}

	/* close the open file and check for errors */
	if (fclose(fp) != 0)
	{
		printf("Unable to close the open file\n");
		return -1;
	}


	//---------------------------------------------------------------- MENU STUFF ------------------------------------------------------------------------
	/* temporary storage for user input related variables */
	char tempUserInput[kMaxParcelString] = "";
	int menuChoice = 0;

	do
	{
		/* Menu */
		printf("Menu:\n");
		printf("1. Display details of each and every parcel\n");
		printf("2. Display lighter or heavier parcels \n");
		printf("3. Display the total weight and valuation of all parcels combined\n");
		printf("4. Display cheapest and most expensive parcel's details\n");
		printf("5. Display lightest and heaviest parcel's details\n");
		printf("6. Exit\n\n");
		printf("Enter your choice: ");
		fgets(tempUserInput, sizeof(tempUserInput), stdin);
		menuChoice = atoi(tempUserInput);

		if (menuChoice < 1 || menuChoice > 5) // invalid menu choice
		{
			if (menuChoice == 6)
			{
				printf("Goodbye!\n\n");
				break;
			}
			else
			{
				printf("Invalid choice!\n\n");
				continue;
			}
		}

		/* initialize menu related variables */
		Parcel* bucket = getCountry(myHashTable);
		int* parcelsPrintedPtr = NULL;
		int* totalWeightPtr = NULL;
		float* totalValuePtr = NULL;
		Parcel* cheapestParcel = NULL;
		Parcel* mostExpensiveParcel = NULL;
		Parcel* lightestParcel = NULL;
		Parcel* HeaviestParcel = NULL;
		bool showNodes = false;
		int option2 = 0; //option 2 special case
		int userWeight = 0;

		if (bucket == NULL) // invalid country (*notice: some strings may generate the same hash as a valid country)
		{
			continue;
		}
		if (menuChoice == 1) // all parcels
		{
			showNodes = true;
			int parcelsPrinted = 0;
			parcelsPrintedPtr = &parcelsPrinted;
			parsePreOrder(bucket, parcelsPrintedPtr, totalWeightPtr, totalValuePtr, &cheapestParcel, &mostExpensiveParcel, &lightestParcel, &HeaviestParcel, showNodes, option2, userWeight);
			printf("\n");
		}
		else if (menuChoice == 2) // lower or higher
		{
			/* get weight & check for validity */
			printf("Enter weight: ");
			char tempUserWeight[kMaxParcelString] = "";
			fgets(tempUserWeight, sizeof(tempUserWeight), stdin);
			if ((strlen(tempUserWeight) > 0) && (strchr(tempUserWeight, '\n') != NULL))
			{
				tempUserWeight[strlen(tempUserWeight) - 1] = '\0';
			}
			int userWeight = atoi(tempUserWeight);
			if (userWeight < kMinParcelWeight || userWeight > kMaxParcelWeight) // invalid range, skip
			{
				printf("\n");
				continue;
			}
			printf("Heavier (1) or Lighter (2)?: ");
			int option2 = getche();
			if (option2 == '1')
			{
				option2 = 1;
			}
			else if (option2 == '2')
			{
				option2 = 2;
			}
			else
			{
				printf("\n\n");
				continue;
			}
			printf("\n\n");

			showNodes = true;
			int parcelsPrinted = 0;
			parcelsPrintedPtr = &parcelsPrinted;
			parsePreOrder(bucket, parcelsPrintedPtr, totalWeightPtr, totalValuePtr, &cheapestParcel, &mostExpensiveParcel, &lightestParcel, &HeaviestParcel, showNodes, option2, userWeight);
			printf("\n");
		}
		else if (menuChoice == 3) // total weight & value
		{
			int totalWeight = 0;
			float totalValue = 0.00;
			totalWeightPtr = &totalWeight;
			totalValuePtr = &totalValue;
			parsePreOrder(bucket, parcelsPrintedPtr, totalWeightPtr, totalValuePtr, &cheapestParcel, &mostExpensiveParcel, &lightestParcel, &HeaviestParcel, showNodes, option2, userWeight);
			printf("Total weight: %d gms\nTotal value:  $%.2f\n\n", totalWeight, totalValue);
			
		}
		else if (menuChoice == 4) // cheapest & most expensive
		{
			cheapestParcel = bucket;
			mostExpensiveParcel = bucket;
			parsePreOrder(bucket, parcelsPrintedPtr, totalWeightPtr, totalValuePtr, &cheapestParcel, &mostExpensiveParcel, &lightestParcel, &HeaviestParcel, showNodes, option2, userWeight);
			printf("Cheapest parcel: %s, %d gms, $%.2f\n", cheapestParcel->destination, cheapestParcel->weight, cheapestParcel->valuation);
			printf("Most expensive parcel: %s, %d gms, $%.2f\n\n", mostExpensiveParcel->destination, mostExpensiveParcel->weight, mostExpensiveParcel->valuation);
			
		}
		else if (menuChoice == 5) // lightest & heaviest
		{
			lightestParcel = bucket;
			HeaviestParcel = bucket;
			parsePreOrder(bucket, parcelsPrintedPtr, totalWeightPtr, totalValuePtr, &cheapestParcel, &mostExpensiveParcel, &lightestParcel, &HeaviestParcel, showNodes, option2, userWeight);
			printf("Lightest parcel: %s, %d gms, $%.2f\n", lightestParcel->destination, lightestParcel->weight, lightestParcel->valuation);
			printf("Heaviest parcel: %s, %d gms, $%.2f\n\n", HeaviestParcel->destination, HeaviestParcel->weight, HeaviestParcel->valuation);
			
		}
		else if (menuChoice == 6) // quit loop
		{
			printf("Goodbye!\n");
			break;
		}
		else // invalid input (*extra precaution)
		{
			printf("Invalid choice!\n\n");
		}
	} while (menuChoice != 6);
	
	/* free BST & hash table */
	for (int i = 0; i < kHashTableSize; i++)
	{
		Parcel* currentParcel = myHashTable->table[i];
		while (currentParcel != NULL)
		{
			Parcel* leftChild = currentParcel->leftChild;
			Parcel* rightChild = currentParcel->rightChild;
			free(currentParcel->destination); // *dynamically allocated country
			free(currentParcel);

			if (leftChild != NULL)
			{
				currentParcel = leftChild;
			}
			else if (rightChild != NULL)
			{
				currentParcel = rightChild;
			}
			else
			{
				break;
			}
		}
	}
	free(myHashTable);

	return 0; // end of main
}



/*
*  Function    : initializeHashTable()
*  Description :
*    Initialize a hash table of type parcel (an array of parcel root nodes).
*  Parameters  :
*    None.
*  Returns     :
*    ParcelHashTable* = a pointer to the head of the hash table.
*/
ParcelHashTable* initializeHashTable(void)
{
	/* dynamically allocate memory for hash table & check for errors */
	ParcelHashTable* hashTable = (ParcelHashTable*)malloc(sizeof(ParcelHashTable));
	if (hashTable == NULL)
	{
		printf("Memory Error!\n");
		exit(EXIT_FAILURE);
	}

	/* initialize all table members with default values */
	for (int i = 0; i < kHashTableSize; i++)
	{
		hashTable->table[i] = NULL;
	}
	return hashTable;
}

/*
*  Function    : initializeParcel()
*  Description :
*    Create a new node of type parcel.
*  Parameters  :
*    char* destination = destination country (e.g. Canada).
*	 int weight        = parcel weight.
*    float valuation   = parcel valuation.
*  Returns     :
*    Parcel* = a pointer to the newly created parcel node.
*/
Parcel* initializeParcel(char* destination, int weight, float valuation)
{
    /* dynamically allocate memory for node */
	Parcel* parcel = (Parcel*)malloc(sizeof(Parcel));
    if (parcel == NULL)
    {
        printf("Memory Error!\n");
        exit(EXIT_FAILURE);
    }

    /* initialize node with values */
	parcel->destination = _strdup(destination); // dynamically allocate destination
	parcel->weight = weight;
	parcel->valuation = valuation;
	parcel->leftChild = NULL;
	parcel->rightChild = NULL;
	//printf("dsllllllllllll");
    return parcel;

}

/*
*  Function    : generateHash()
*  Description :
*    Generate a hash value based on a string argument.
*  Parameters  :
*    char* str = string to hash.
*  Returns     :
*    unsigned long = hash value generated.
*/
unsigned long generateHash(char* str)
{
	/* hashing algorithm */
	unsigned long hash = 5381;
	int c = 0;
	while ((c = *str++) != '\0')
	{
		hash = ((hash << 5) + hash) + c;
	}
	return hash % kHashTableSize; // restrict output into valid buckets (0-127)
}

/*
*  Function    : insertIntoTable()
*  Description :
*    Insert a node into a bucket in the hash table based on its hash value.
*  Parameters  :
*    ParcelHashTable* hashTable = the hash table to insert into.
*    char* destination          = destination country (will be used to determine hash).
*	 int weight                 = parcel weight.
*    float valuation            = parcel valuation.
*  Returns     :
*    None.
*/
void insertIntoTable(ParcelHashTable* hashTable, char* destination, int weight, float valuation)
{
	int hash = generateHash(destination); // generate hash based on destination country
	hashTable->table[hash] = insertSorted(hashTable->table[hash], destination, weight, valuation); // insert parcel into its appropriate bucket //self-note see if u can delete "hashTable->table[hash] = "
}

/*
*  Function    : insertSorted()
*  Description :
*    Create and insert a new parcel node into a BST structure.
*  Parameters  :
*    Parcel* parent    = the root node of the BST.
*    char* destination = destination country.
*	 int weight        = parcel weight (will be used to determine BST structure).
*	 float valuation   = parcel valuation.
*  Returns     :
*    Parcel* = the newly created parcel node.
*/
Parcel* insertSorted(Parcel* parent, char* destination, int weight, float valuation)
{
	/* node is empty */
	if (parent == NULL) 
	{
		return initializeParcel(destination, weight, valuation); // create node;
	}

	if (weight < parent->weight)
	{
		parent->leftChild = insertSorted(parent->leftChild, destination, weight, valuation);
	}
	else if (weight > parent->weight)
	{
		parent->rightChild = insertSorted(parent->rightChild, destination, weight, valuation);
	}

	return parent;
}

/*
*  Function    : whichBucketToSearch()
*  Description :
*    Take a name and return which bucket to traverse.
*  Parameters  :
*    ParcelHashTable* hashTable = the hash table with the buckets.
*    char* str                  = the string to look for.
*  Returns     :
*    Parcel* = a pointer the bucket to traverse.
*/
Parcel* whichBucketToSearch(ParcelHashTable* hashTable, char* str)
{
	/* determine which bucket to search in */
	int hash = generateHash(str);
	Parcel* bucket = hashTable->table[hash]; // choose bucket based on hash
	return bucket;
}

/*
*  Function    : getCountry()
*  Description :
*    Ask for user input for a country and see in which bucket it is.
*  Parameters  :
*    ParcelHashTable* hashTable = the hash table with the buckets.
*  Returns     :
*    Parcel* = a pointer the bucket based on the country.
*/
Parcel* getCountry(ParcelHashTable* hashTable)
{
	/* get country from user */
	printf("Enter country name: ");
	char tempUserCountry[kMaxParcelString] = "";
	fgets(tempUserCountry, sizeof(tempUserCountry), stdin);
	printf("\n");
	/* remove newline character (\n) */
	if ((strlen(tempUserCountry) > 0) && (strchr(tempUserCountry, '\n') != NULL))
	{
		tempUserCountry[strlen(tempUserCountry) - 1] = '\0';
	}

	Parcel* bucket = whichBucketToSearch(hashTable, tempUserCountry);
	return bucket;
}

/*
*  Function    : parsePreOrder()
*  Description :
*    This function parses through a BST and extracts valuable information along the way giving the user control over them.
*  Parameters  :
*    Parcel* root				  = the root node of the tree.
	 int* parcelsPrinted		  = the number of parcels printed.
	 int* totalWeight			  = the total weight of all parcels.
	 float* totalValue			  = the total value of all parcels.
	 Parcel** cheapestParcel	  = cheapest parcel found.
	 Parcel** mostExpensiveParcel = most expensive parcel found.
	 Parcel** lightestParcel	  = lightest parcel found.
	 Parcel** heaviestParcel	  = heaviest parcel found.
	 bool printNodes			  = print nodes YES/NO?
	 int heavier				  = heavier/lighter parcels only? (for case 2)
	 int amount					  = weight (for case 2)
*  Returns     :
*    None.
*/
void parsePreOrder(Parcel* root, int* parcelsPrinted, int* totalWeight, float* totalValue, Parcel** cheapestParcel, Parcel** mostExpensiveParcel, Parcel** lightestParcel, Parcel** heaviestParcel, bool printNodes, int heavier, int amount)
{
	if (root == NULL)
	{
		return;
	}

	/* extract value & weight */
	if (totalWeight != NULL && totalValue != NULL)
	{
		*totalWeight += root->weight;
		*totalValue += root->valuation;
	}

	/* extract cheapest & most expensive */
	if (*cheapestParcel != NULL && *mostExpensiveParcel != NULL)
	{
		if (root->valuation < (*cheapestParcel)->valuation) //1
		{
			*cheapestParcel = root;

		}
		if (root->valuation > (*mostExpensiveParcel)->valuation)
		{
			*mostExpensiveParcel = root;
		}
	}

	/* extract lightest & heaviest */
	if (*lightestParcel != NULL && *heaviestParcel != NULL)
	{
		if (root->weight < (*lightestParcel)->weight) //2
		{
			*lightestParcel = root;
		}
		if (root->weight > (*heaviestParcel)->weight)
		{
			*heaviestParcel = root;
		}
	}

	/* print node based on specified options */
	if (printNodes && (heavier == 1 || heavier == 2))
	{
		if (heavier == 1 && root->weight > amount)
		{
			printf("%-2d. %s, %-5d gms, $%.2f\n", ++*parcelsPrinted, root->destination, root->weight, root->valuation);
		}
		else if (heavier == 2 && root->weight < amount)
		{
			printf("%-2d. %s, %-5d gms, $%.2f\n", ++*parcelsPrinted, root->destination, root->weight, root->valuation);
		}

	}
	else if (printNodes)
	{
		printf("%-2d. %s, %-5d gms, $%.2f\n", ++*parcelsPrinted, root->destination, root->weight, root->valuation);
	}

	parsePreOrder(root->leftChild, parcelsPrinted, totalWeight, totalValue, cheapestParcel, mostExpensiveParcel, lightestParcel, heaviestParcel, printNodes, heavier, amount); // traverse left
	parsePreOrder(root->rightChild, parcelsPrinted, totalWeight, totalValue, cheapestParcel, mostExpensiveParcel, lightestParcel, heaviestParcel, printNodes, heavier, amount); // traverse right
}

