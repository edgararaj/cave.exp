/*#include<time.h>
#include<stdio.h>
#include<stdlib>
*/

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

DeternmineNextMove(monsterPosition, playerPosition)
{

    int distance = calculateDistance(monsterPosition, playerPosition);

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

int main()
{
    srand(time(NULL));

    Monster randomMonster = generateMoster();

    printf("Generated Monster:\n");
    printf("Hp: %d\n");
    printf("Dmg: %d\n");
    printf("Speed: %d\n", randomMonster);

    return 0;
}
