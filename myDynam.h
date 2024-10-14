#include <iostream>

enum entityType{ PLANT, ZOMBIE, PROJECTILE };
enum plants{ SUNFLOWER, PEASHOOTER, LANDMINE };
enum zombies { NORMAL, CONE, BUCKET, SPRINTER };
enum projectiles { PEA };


struct Entity{
    entityType type;
    int subType;
    int health;
    int speed;
    int turnMeter;
    int damage;
    bool abilityUsed;
    int x, y;
};

using namespace std;
void myPop(int* &arr, int &N, int elemToPop){
    if (elemToPop >= N || elemToPop < 0){
        return;
    }

    int* oldArr = arr;
    arr = new int[N-1];

    int temp = 0;
    for(int i = 0; i < N; i++){
        if(i != elemToPop){
            arr[temp] = oldArr[i];
            temp++;
        }
    }
    delete[] oldArr;
    N--;
    return;
}

int uses = 0;
void myPop(Entity* &arr, int &N, int elemToPop){
    if (elemToPop >= N || elemToPop < 0){
        return;
    }

    Entity* oldArr = arr;
    arr = new Entity[N-1];

    int temp = 0;
    for(int i = 0; i < N; i++){
        if(i != elemToPop){
            arr[temp] = oldArr[i];
            temp+=1;
        }
    }

    delete[] oldArr;
    N--;
    
}

void myInsert(int* &arr, int &N, int elemToInsert, int index){
    if (index >= N || index < 0){
        return;
    }

    int* oldArr = arr;
    arr = new int[N+1];
    N++;

    int temp = 0;
    for(int i = 0; i < N; i++){
        if(i != index){
            arr[i] = oldArr[temp];
            temp++;
        } else {
            arr[i] = elemToInsert;
        }
    }

    delete[] oldArr;
    return;
}
void myInsert(Entity* &arr, int &N, Entity elemToInsert, int index){
    if (index >= N || index < 0){
        return;
    }

    Entity* oldArr = arr;
    arr = new Entity[N+1];
    N++;

    int temp = 0;
    for(int i = 0; i < N; i++){
        if(i != index){
            arr[i] = oldArr[temp];
            temp++;
        } else {
            arr[i] = elemToInsert;
        }
    }

    delete[] oldArr;
    return;
}

void myAppend(Entity* &arr, int &N, Entity elemToInsert){
    Entity *oldArr = arr;
    arr = new Entity[N+1];
    for(int i = 0; i < N; i++){
        arr[i] = oldArr[i];
    }

    arr[N] = elemToInsert;
    N++;
    delete[] oldArr;
}

void myAppend(int** &arr, int &N, int* elemToInsert){
    int **oldArr = arr;
    arr = new int*[N+1];
    
    for(int i = 0; i < N; i++){
        arr[i] = oldArr[i];
    }

    arr[N] = elemToInsert;
    N++;
    
    delete[] oldArr;  // Free the old array (not the rows it points to)
}


void startInsert(int* &arr, int &N, int elemToInsert){
    myInsert(arr, N, elemToInsert, 0);
}

void deleteLast(int* &arr, int &N, int elemToInsert){
    myPop(arr, N, N-1);
}

void deleteMiddle(int* &arr, int &N, int elemToInsert){
    myPop(arr, N, (N/2)-1);
}