#include<iostream>
#include<stdlib.h>


using namespace std;


int const InsideCount = 6;//内存中存放的页面数
int count_ = 0;
int Inside[InsideCount];



int const PageCount   =10;//总的页面数
int Page[PageCount];


int insert = 0;//先到先出置换算法fcfo中表示 当内存满的时候，新进入的页号放的位置
int suiji   = 0; //随机置换算法randchange  当内存满的时候，新进入的页号放的位置

int state[InsideCount];//clock置换算法中，内存中的每个页面号对应的状态
int state2[InsideCount][2];// 二维数组，第一行第一列为访问位，第一行的第二列为修改位

double lost = 0.0;

bool isInside(int num)//检测页号是否在内存中
{
    for (int i = 0; i < InsideCount; i++)
    {
        if (Inside[i] == Page[num])
        {
            state[i] = 1;
            return true;
        }
    }

    return false;
}

bool change()     //判断页面是否已经被修改
{
    if ((rand()%2+1) == 1 )
    {
        cout<<"该页面被修改"<<endl;
        return true;
    }
    else
        return false;
}


bool isInside2(int num)//用于改进型clock置换算法，检测页号是否在内存中并把访问位和修改位置1
{
    for (int i = 0; i < InsideCount; i++)
    {
        if (Inside[i] == Page[num])
        {
            if (change())
            {
                state2[i][0] = 1;
                state2[i][1] = 1;
            }
            else
            {
                state2[i][0] = 1;
            }
            return true;
        }
    }

    return false;
}


int whichpage()//用于改进型clock置换算法，判断内存中第几个需要被置换
{
    int j;


    for (j=0; j < InsideCount;j++ )
    {
        if (state2[j][0] == 0&&state2[j][1] == 0)
        {

            return j;

        }
    }

    for (j=0; j < InsideCount;j++ )
    {
        if (state2[j][0] == 0&&state2[j][1] == 1)
        {

            return j;

        }
        state2[j][0] = 0 ;

    }
    for (j=0; j < InsideCount;j++ )
    {
        state2[j][0] = 0 ;

    }

    return whichpage();

}
void improveclock(int num) //改进型clock置换算法
{
    int j;

    if (isInside2(num))
    {
        cout<<"命中"<<endl;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }
    else if (count_ == InsideCount)
    {
        lost++;

        j =  whichpage();


        Inside[j] = Page[num];
        state2[j][0] = 1;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }
    else
    {
        //lost++;
        Inside[count_] = Page[num];
        //state2[count][0] = 1;
        count_++;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }

}
void clock(int num)//简单Clock置换算法
{
    int j;

    if (isInside(num))
    {
        cout<<"命中"<<endl;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }

    else
        if (count_ == InsideCount)
        {
            lost++;
            /*for(int j=0; j < InsideCount; j++)
              {
              for( k = num; k >0;k--)   //从当前的页号向前看，发现页号与内存中的页号相同，break ；比较内存中三个页号，看哪一个走的远，用max记录
              {
              if(Inside[j] == Page[k])
              break;
              }

              if(  num - k > max)
              {
              max = num - k;
              maxchange =j;//j 表示把 内存中第j个Inside中的页面从内存拿出，把新的页面放入
              }
              }*/
            for (j=0; j < InsideCount; )
            {
                if (state[j] == 0)
                {
                    break;
                }

                else
                {
                    state[j] = 0;


                }

                j++;

                j = j %3;

            }

            Inside[j] = Page[num];
            state[j] = 1;

            for (int i=0 ; i <InsideCount; i++)
                cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
        }
        else
        {
            //lost++;
            Inside[count_] = Page[num];
            //state[count] = 1;
            count_++;

            for (int i=0 ; i <InsideCount; i++)
                cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
        }


}
void LRU(int num)//最近最久未使用置换算法LRU
{
    int max = 0; // 表示内存中的页号，下一次出现的距离
    int maxchange; //表示内存中下次出现距离最大的页号在内存中的位置

    int k;

    if (isInside(num))
    {
        cout<<"命中"<<endl;
        for (int i=0 ; i <InsideCount; i++)
            cout<<"内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }

    else
        if (count_ == InsideCount)
        {
            lost++;
            for (int j=0; j < InsideCount; j++)
            {
                for ( k = num; k >0;k--)  //从当前的页号向前看，发现页号与内存中的页号相同，break ；比较内存中三个页号，看哪一个走的远，用max记录
                {
                    if (Inside[j] == Page[k])
                        break;
                }

                if (  num - k > max)
                {
                    max = num - k;
                    maxchange =j;//j 表示把 内存中第j个Inside中的页面从内存拿出，把新的页面放入
                }
            }

            Inside[maxchange] = Page[num];

            for (int i=0 ; i <InsideCount; i++)
                cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
        }
        else
        {
            //lost++;
            Inside[count_] = Page[num];
            count_++;

            for (int i=0 ; i <InsideCount; i++)
                cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
        }
}
void randexchange(int num)//随机置换算法
{

    if (isInside(num))
    {
        cout<<"命中"<<endl;
        for (int i=0 ; i <InsideCount; i++)
            cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }

    else
        if (count_ == InsideCount)
        {
            lost++;
            suiji = rand()%3;
            Inside[suiji] = Page[num];
            //cout<<suiji<<"111111111111111"<<endl;


            for (int i=0 ; i <InsideCount; i++)
                cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
        }
        else
        {
            //lost++;
            Inside[count_] = Page[num];
            count_++;

            for (int i=0 ; i <InsideCount; i++)
                cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
        }


}


void FIFO(int num)//先进现出置换算法（FIFO）：
{
    if (isInside(num))
    {
        cout<<"命中"<<endl;
        for (int i=0 ; i <InsideCount; i++)
            cout<<"  内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }

    else
        if (count_ == InsideCount)
        {
            lost++;
            Inside[insert] = Page[num];
            insert++;

            insert = insert % 3;
            for (int i=0 ; i <InsideCount; i++)
                cout<<"  内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
        }
        else
        {
            //lost++;
            Inside[count_] = Page[num];
            count_++;

            for (int i=0 ; i <InsideCount; i++)
                cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
        }
}




void OPT(int num)//最佳置换算法（OPT）（理想置换算法）
{
    int max = 0; // 表示内存中的页号，下一次出现的距离
    int maxchange; //表示内存中下次出现距离最大的页号在内存中的位置

    int k;

    if (isInside(num))
    {
        cout<<"命中"<<endl;
        for (int i=0 ; i <InsideCount; i++)
            cout<<"内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }

    else if (count_ == InsideCount)
    {
        lost++;
        for (int j=0; j < InsideCount; j++)
        {
            for ( k = num; k < PageCount;k++)
            {
                if (Inside[j] == Page[k])
                    break;
            }

            if ( k > max)
            {
                max = k;    //k表示在这个地方会再次出现给定页面
                maxchange =j;//j 表示把 内存中第j个Inside中的页面从内存拿出，把新的页面放入
            }
        }

        Inside[maxchange] = Page[num];

        for (int i=0 ; i <InsideCount; i++)
            cout<<"  内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }
    else
    {
        //lost++;
        Inside[count_] = Page[num];
        count_++;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" 内存Inside["<<i<<"]中内容为："<<Inside [i]<<endl;
    }
}


int main()
{
    char ch ;//选择使用哪种算法

    while (1)
    {
        for (int i = 0; i < PageCount; i++)
        {
            Page[i] =rand()%9 + 1;

        }

        cout<<"O 最佳置换算法（OPT）（理想置换算法）"<<"\n"<<endl;
        cout<<"F 先进现出置换算法（FIFO）"<<"\n"<<endl;
        cout<<"S 表示最随机置换算法"<<"\n"<<endl;
        cout<<"U 最近最久未使用（LRU）算法"<<"\n"<<endl;
        cout<<"C 简单Clock置换算法"<<"\n"<<endl;
        cout<<"I 改进型clock置换算法"<<"\n"<<endl;

        cin>>ch;

        switch (ch)
        {
        case 'O':
        {
            lost = 0;
            count_ = 0;
            for (int j = 0; j < InsideCount; j++)
            {
                Inside[j] = 0;

            }
            for (int i = 0; i < PageCount; i++)
            {
                cout<<"读入   Page["<<i<<"]="<<Page[i]<<endl;
                OPT(i);
            }

            cout<<"共 " <<PageCount<<"次   ,"<<"缺失 "<<lost<<" 次，缺失率为"<<lost/(PageCount)<<"\n\n"<<endl;
        }
        break;
        case 'F':
        {
            lost =  0;
            count_ = 0;
            for (int j = 0; j < InsideCount; j++)
            {
                Inside[j] = 0;

            }
            for (int i = 0; i < PageCount; i++)
            {
                cout<<"读入   Page["<<i<<"]="<<Page[i]<<endl;
                FIFO(i);
            }

            cout<<"共 " <<PageCount<<"次   ,"<<"缺失 "<<lost<<" 次，缺失率为"<<lost/(PageCount)<<"\n\n"<<endl;

        }
        break;
        case 'S':
        {
            lost = 0;
            count_ = 0;
            for (int j = 0; j < InsideCount; j++)
            {
                Inside[j] = 0;

            }
            for (int i = 0; i < PageCount; i++)
            {
                cout<<"读入 Page["<<i<<"]="<<Page[i]<<endl;
                randexchange(i);
            }

            cout<<"共 " <<PageCount<<"次   ,"<<"缺失 "<<lost<<" 次，缺失率为"<<lost/(PageCount)<<"\n\n"<<endl;

        }

        break;


        case 'U':
        {
            lost = 0;
            count_ = 0;
            for (int j = 0; j < InsideCount; j++)
            {
                Inside[j] = 0;

            }
            for (int i = 0; i < PageCount; i++)
            {
                cout<<"读入  Page["<<i<<"]="<<Page[i]<<endl;
                LRU(i);
            }

            cout<<"共 " <<PageCount<<"次   ,"<<"缺失 "<<lost<<" 次，缺失率为"<<lost/(PageCount)<<"\n\n"<<endl;

        }
        break;
        case 'C':
        {
            lost = 0;
            count_ = 0;

            for (int m = 0; m < InsideCount; m++)
            {
                state[m] = 0;
            }

            for (int j = 0; j < InsideCount; j++)
            {
                Inside[j] = 0;
            }

            for (int i = 0; i < PageCount; i++)
            {
                cout<<"读入  Page["<<i<<"]="<<Page[i]<<endl;
                clock(i);
            }

            cout<<"共 " <<PageCount<<"次   ,"<<"缺失 "<<lost<<" 次，缺失率为"<<lost/(PageCount)<<"\n\n"<<endl;
        }
        break;

        case 'I':
        {
            lost = 0;
            count_ = 0;

            for (int m = 0; m < InsideCount; m++)
            {
                for (int n = 0; n < 2;n++)
                    state2[m][n] = 0;
            }

            for (int j = 0; j < InsideCount; j++)
            {
                Inside[j] = 0;
            }

            for (int i = 0; i < PageCount; i++)
            {
                cout<<"读入 Page["<<i<<"]="<<Page[i]<<endl;
                improveclock(i);
            }

            cout<<"共 " <<PageCount<<"次   ,"<<"缺失 "<<lost<<" 次，缺失率为"<<lost/(PageCount)<<"\n\n"<<endl;

        }
        break;
        }
    }
    return 0;
}
