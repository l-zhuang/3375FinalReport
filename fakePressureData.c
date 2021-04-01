
int c1=0;
int retrivePData(){
    if(c1!=len(fakePData)){
        return fakePData[c1];
        c1++;
    }
    else{
        return fakePData[c1];
        c1=0;
    }
}

int fakePData[]={1,15,20,17,20,6,7,3,16,18,19,2,4,9,12,11};