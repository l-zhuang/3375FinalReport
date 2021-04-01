int c=0;
int retriveWData(){
    if(c!=len(fakeData)){
        return fakeData[c];
        c++;
    }
    else{
        return fakeData[c];
        c=0;
    }
}

int fakeData[]={3,-4,5,6,4,-5,-3,2,4,-3,2,-3,2,3,-2,4,4,3,-2,1,3,2,4};
int n;
