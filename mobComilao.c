

typedef struct monster
{
    int hp;
    int dmg;
    int speed;
} Monster;

Monster generateMoster()
{
    Monster monster;

    monster.hp = 10;
    monster.dmg = 2;
    monster.speed = 0, 80;

    return monster;
}

typedef enum
{
    Wander,
    Chase,
    Attack,
} Behavior;


DeternmineNextMove(int monsterPosition, int playerPosition)
{

    int distance = abs(monsterPosition - playerPosition);

    if (distance > 10)
    {
        return Wander;
    }
    else if (distance > 5)
    {
        return Chase;
    }
    else if (distance >= 2)
    {
        return Attack;
    }
}


void Wander(Monster *monster)
{
    int direction = rand() % 2; //se o resto for 1 = direita se for 0 = esquerda

    //Update na localizaçao do bicho
    if(direction == 0)
    {
        printf("The monster is wandering left");
    } else {
        printf("The monster is Wandering to the right");
    }
    /*esta parte parece um pouco complicada sao as interaçoes*/
}

void Chase(Monster *monster, int playerPosition)
{
    if(monster -> position < playerPosition)
    {
        printf("Monster is chasing to the right");

        monster->position = monster->position + playerPosition; 

    } else if(monster->position > playerPosition)
    {
        printf("Monster is chasing to the left");

        monster->position = monster->position - playerPosition;

    } else {
        printf("The monster is going to eat");
    }
    /*esta parte parece um pouco complicada sao as interaçoes*/
}

void Attack(Monster *monster, int playerPosition, int *player_hp)
{
    if(monsterPosition == playerPosition)
    {
        *player_hp = -1;
    }
}



int main()
{
    srand(time(NULL));

    Monster comilaoMonster = generateMoster();

    printf("Generated Monster:\n");
    printf("Hp: %d\n", comilaoMonster);
    printf("Dmg: %d\n", comilaoMonster);
    printf("Speed: %d\n", comilaoMonster);

    return 0;
}
