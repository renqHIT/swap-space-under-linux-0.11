#include<iostream>
#include<stdlib.h>


using namespace std;


int const InsideCount = 6;//�ڴ��д�ŵ�ҳ����
int count_ = 0;
int Inside[InsideCount];



int const PageCount   =10;//�ܵ�ҳ����
int Page[PageCount];


int insert = 0;//�ȵ��ȳ��û��㷨fcfo�б�ʾ ���ڴ�����ʱ���½����ҳ�ŷŵ�λ��
int suiji   = 0; //����û��㷨randchange  ���ڴ�����ʱ���½����ҳ�ŷŵ�λ��

int state[InsideCount];//clock�û��㷨�У��ڴ��е�ÿ��ҳ��Ŷ�Ӧ��״̬
int state2[InsideCount][2];// ��ά���飬��һ�е�һ��Ϊ����λ����һ�еĵڶ���Ϊ�޸�λ

double lost = 0.0;

bool isInside(int num)//���ҳ���Ƿ����ڴ���
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

bool change()     //�ж�ҳ���Ƿ��Ѿ����޸�
{
    if ((rand()%2+1) == 1 )
    {
        cout<<"��ҳ�汻�޸�"<<endl;
        return true;
    }
    else
        return false;
}


bool isInside2(int num)//���ڸĽ���clock�û��㷨�����ҳ���Ƿ����ڴ��в��ѷ���λ���޸�λ��1
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


int whichpage()//���ڸĽ���clock�û��㷨���ж��ڴ��еڼ�����Ҫ���û�
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
void improveclock(int num) //�Ľ���clock�û��㷨
{
    int j;

    if (isInside2(num))
    {
        cout<<"����"<<endl;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
    }
    else if (count_ == InsideCount)
    {
        lost++;

        j =  whichpage();


        Inside[j] = Page[num];
        state2[j][0] = 1;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
    }
    else
    {
        //lost++;
        Inside[count_] = Page[num];
        //state2[count][0] = 1;
        count_++;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
    }

}
void clock(int num)//��Clock�û��㷨
{
    int j;

    if (isInside(num))
    {
        cout<<"����"<<endl;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
    }

    else
        if (count_ == InsideCount)
        {
            lost++;
            /*for(int j=0; j < InsideCount; j++)
              {
              for( k = num; k >0;k--)   //�ӵ�ǰ��ҳ����ǰ��������ҳ�����ڴ��е�ҳ����ͬ��break ���Ƚ��ڴ�������ҳ�ţ�����һ���ߵ�Զ����max��¼
              {
              if(Inside[j] == Page[k])
              break;
              }

              if(  num - k > max)
              {
              max = num - k;
              maxchange =j;//j ��ʾ�� �ڴ��е�j��Inside�е�ҳ����ڴ��ó������µ�ҳ�����
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
                cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
        }
        else
        {
            //lost++;
            Inside[count_] = Page[num];
            //state[count] = 1;
            count_++;

            for (int i=0 ; i <InsideCount; i++)
                cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
        }


}
void LRU(int num)//������δʹ���û��㷨LRU
{
    int max = 0; // ��ʾ�ڴ��е�ҳ�ţ���һ�γ��ֵľ���
    int maxchange; //��ʾ�ڴ����´γ��־�������ҳ�����ڴ��е�λ��

    int k;

    if (isInside(num))
    {
        cout<<"����"<<endl;
        for (int i=0 ; i <InsideCount; i++)
            cout<<"�ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
    }

    else
        if (count_ == InsideCount)
        {
            lost++;
            for (int j=0; j < InsideCount; j++)
            {
                for ( k = num; k >0;k--)  //�ӵ�ǰ��ҳ����ǰ��������ҳ�����ڴ��е�ҳ����ͬ��break ���Ƚ��ڴ�������ҳ�ţ�����һ���ߵ�Զ����max��¼
                {
                    if (Inside[j] == Page[k])
                        break;
                }

                if (  num - k > max)
                {
                    max = num - k;
                    maxchange =j;//j ��ʾ�� �ڴ��е�j��Inside�е�ҳ����ڴ��ó������µ�ҳ�����
                }
            }

            Inside[maxchange] = Page[num];

            for (int i=0 ; i <InsideCount; i++)
                cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
        }
        else
        {
            //lost++;
            Inside[count_] = Page[num];
            count_++;

            for (int i=0 ; i <InsideCount; i++)
                cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
        }
}
void randexchange(int num)//����û��㷨
{

    if (isInside(num))
    {
        cout<<"����"<<endl;
        for (int i=0 ; i <InsideCount; i++)
            cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
    }

    else
        if (count_ == InsideCount)
        {
            lost++;
            suiji = rand()%3;
            Inside[suiji] = Page[num];
            //cout<<suiji<<"111111111111111"<<endl;


            for (int i=0 ; i <InsideCount; i++)
                cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
        }
        else
        {
            //lost++;
            Inside[count_] = Page[num];
            count_++;

            for (int i=0 ; i <InsideCount; i++)
                cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
        }


}


void FIFO(int num)//�Ƚ��ֳ��û��㷨��FIFO����
{
    if (isInside(num))
    {
        cout<<"����"<<endl;
        for (int i=0 ; i <InsideCount; i++)
            cout<<"  �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
    }

    else
        if (count_ == InsideCount)
        {
            lost++;
            Inside[insert] = Page[num];
            insert++;

            insert = insert % 3;
            for (int i=0 ; i <InsideCount; i++)
                cout<<"  �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
        }
        else
        {
            //lost++;
            Inside[count_] = Page[num];
            count_++;

            for (int i=0 ; i <InsideCount; i++)
                cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
        }
}




void OPT(int num)//����û��㷨��OPT���������û��㷨��
{
    int max = 0; // ��ʾ�ڴ��е�ҳ�ţ���һ�γ��ֵľ���
    int maxchange; //��ʾ�ڴ����´γ��־�������ҳ�����ڴ��е�λ��

    int k;

    if (isInside(num))
    {
        cout<<"����"<<endl;
        for (int i=0 ; i <InsideCount; i++)
            cout<<"�ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
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
                max = k;    //k��ʾ������ط����ٴγ��ָ���ҳ��
                maxchange =j;//j ��ʾ�� �ڴ��е�j��Inside�е�ҳ����ڴ��ó������µ�ҳ�����
            }
        }

        Inside[maxchange] = Page[num];

        for (int i=0 ; i <InsideCount; i++)
            cout<<"  �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
    }
    else
    {
        //lost++;
        Inside[count_] = Page[num];
        count_++;

        for (int i=0 ; i <InsideCount; i++)
            cout<<" �ڴ�Inside["<<i<<"]������Ϊ��"<<Inside [i]<<endl;
    }
}


int main()
{
    char ch ;//ѡ��ʹ�������㷨

    while (1)
    {
        for (int i = 0; i < PageCount; i++)
        {
            Page[i] =rand()%9 + 1;

        }

        cout<<"O ����û��㷨��OPT���������û��㷨��"<<"\n"<<endl;
        cout<<"F �Ƚ��ֳ��û��㷨��FIFO��"<<"\n"<<endl;
        cout<<"S ��ʾ������û��㷨"<<"\n"<<endl;
        cout<<"U ������δʹ�ã�LRU���㷨"<<"\n"<<endl;
        cout<<"C ��Clock�û��㷨"<<"\n"<<endl;
        cout<<"I �Ľ���clock�û��㷨"<<"\n"<<endl;

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
                cout<<"����   Page["<<i<<"]="<<Page[i]<<endl;
                OPT(i);
            }

            cout<<"�� " <<PageCount<<"��   ,"<<"ȱʧ "<<lost<<" �Σ�ȱʧ��Ϊ"<<lost/(PageCount)<<"\n\n"<<endl;
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
                cout<<"����   Page["<<i<<"]="<<Page[i]<<endl;
                FIFO(i);
            }

            cout<<"�� " <<PageCount<<"��   ,"<<"ȱʧ "<<lost<<" �Σ�ȱʧ��Ϊ"<<lost/(PageCount)<<"\n\n"<<endl;

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
                cout<<"���� Page["<<i<<"]="<<Page[i]<<endl;
                randexchange(i);
            }

            cout<<"�� " <<PageCount<<"��   ,"<<"ȱʧ "<<lost<<" �Σ�ȱʧ��Ϊ"<<lost/(PageCount)<<"\n\n"<<endl;

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
                cout<<"����  Page["<<i<<"]="<<Page[i]<<endl;
                LRU(i);
            }

            cout<<"�� " <<PageCount<<"��   ,"<<"ȱʧ "<<lost<<" �Σ�ȱʧ��Ϊ"<<lost/(PageCount)<<"\n\n"<<endl;

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
                cout<<"����  Page["<<i<<"]="<<Page[i]<<endl;
                clock(i);
            }

            cout<<"�� " <<PageCount<<"��   ,"<<"ȱʧ "<<lost<<" �Σ�ȱʧ��Ϊ"<<lost/(PageCount)<<"\n\n"<<endl;
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
                cout<<"���� Page["<<i<<"]="<<Page[i]<<endl;
                improveclock(i);
            }

            cout<<"�� " <<PageCount<<"��   ,"<<"ȱʧ "<<lost<<" �Σ�ȱʧ��Ϊ"<<lost/(PageCount)<<"\n\n"<<endl;

        }
        break;
        }
    }
    return 0;
}
