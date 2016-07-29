#include<iostream>
#include<string.h>
using std::cout;
using std::endl;


//copy start here!!
#define LISTNUMBER 5
#define CUCKOO_DONE 1
#define calGroups(X) (X+LISTNUMBER-1)/LISTNUMBER


void theAL(int index, int a[], int mCount) {
    if(index<=0||index>mCount||a==NULL) {
        cout<<"Wrong para!!";
        return;
    }

    int mIndex=index-1;
    int headGroupNumber=LISTNUMBER;
    int preGroups = calGroups(mIndex);
	int postGroups;
    if(mIndex+LISTNUMBER<=mCount) {
        postGroups = calGroups(mCount-(mIndex+LISTNUMBER));
    } else {
        postGroups = 0;
        headGroupNumber=mCount-mIndex;
    }

    int * tempArray = new int[mCount];
    memset(tempArray,-1,mCount*sizeof(int));

    for(int i= 0; i<mCount; i++) {
        int target=0;
        if(i>=mIndex+LISTNUMBER) {
            //in post groups
            int order;
            order=calGroups(i-(mIndex+LISTNUMBER-1));
            if(order<=preGroups) {
                target = headGroupNumber + 2*LISTNUMBER*(order-1)+(i-(mIndex+LISTNUMBER))%LISTNUMBER;
            } else {
                target=i;
            }
        } else if(i<mIndex) {
            //in pre groups
            int order;
            order=calGroups(mIndex-i);
            if(order<postGroups) {
                target=headGroupNumber+2*LISTNUMBER*(order-1)+LISTNUMBER+(mIndex-i-1)%LISTNUMBER;
            } else {
                target=mCount-1-i;
            }
        } else {
            //in head group
            target=(i-mIndex)%LISTNUMBER;
        }
        tempArray[target]=a[i];
    }

    for(int i=0; i<mCount; i++) {
        a[i]=tempArray[i];
    }
    delete [] tempArray;
}

//copy end!!!!


void testAL(int index,int count) {
    int a[50]= {0};
    for(int i=0; i<count; i++) {
        a[i]=i+1;
    }

    theAL(index, a ,count);

    for(int i=0; i<count; i++) {
        cout<<a[i]<<endl;
        if((i+1)%LISTNUMBER==0)
            cout<<"_____________________________________________________________"<<endl;
    }
}
int main(int argc, char** argv) {
    int index = 39;
    int count = 40;
    cout<<"index = "<<index<<endl
        <<"count = "<<count<<endl;
    testAL(index, count);
    return 0;
}




/*
int mCount;
int mIndex;
int headGroupNumber=LISTNUMBER;
int preGroups;
int postGroups;


//calculate the targetPosition
int targetPosition(int orign) {
    int target=0;
    if(orign>=mIndex+LISTNUMBER) {
        //in post groups
        int order;
        order=calGroups(orign-(mIndex+LISTNUMBER-1));
        if(order<=preGroups) {
            target = headGroupNumber + 2*LISTNUMBER*(order-1)+(orign-(mIndex+LISTNUMBER))%LISTNUMBER;
        } else {
            target=orign;
        }
    } else if(orign<mIndex) {
        //in pre groups
        int order;
        order=calGroups(mIndex-orign);
        if(order<postGroups) {
            target=headGroupNumber+2*LISTNUMBER*(order-1)+LISTNUMBER+(mIndex-orign-1)%LISTNUMBER;
        } else {
            target=mCount-1-orign;
        }
    } else {
        //in head group
        target=(orign-mIndex)%LISTNUMBER;
    }

    return target;
}
*/