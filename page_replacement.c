#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define FILENAMESIZE 500
#define BUFSIZE 300
#define PRSSIZE 30

void print_info(char *algoname, int *prs, int prsNum, int frameNum);
void OPT(int frameNum, int *prs, int prsNum);//Optimal Algorithm 수행
void LRU(int frameNum, int *prs, int prsNum);
void FIFO(int frameNum, int *prs, int prsNum);//First-in-First-out 알고리즘 수행
void SecondChance(int frameNum, int *prs, int prsNum);//Second-Chance 알고리즘 수행


int main(){
	FILE * fp;
	char fileName[FILENAMESIZE], buf[BUFSIZE];
	int prs[PRSSIZE];
	int frameNum, prsNum = 0, prsLen;
	char *ptr;

	memset(fileName, 0, FILENAMESIZE);
	printf("파일 명을 입력 >> ");
	scanf("%s", fileName);

	if((fp = fopen(fileName, "r")) == NULL){
		fprintf(stderr ,"fopen error for %s\n", fileName);
		exit(1);
	}
	
	memset(buf, 0, BUFSIZE);
	fgets(buf, BUFSIZE, fp);
	frameNum = atoi(buf);
	
	memset(buf, 0, BUFSIZE);
	fgets(buf, BUFSIZE, fp);
	buf[strlen(buf)-1] = '\0';
	
	ptr = strtok(buf, " ");
	while(ptr != NULL){
		prs[prsNum++] = atoi(ptr);
		ptr = strtok(NULL, " ");
	}

	if((prsNum < 1) || (prsNum > 30) || (frameNum < 1) || (frameNum > 4)){
		fprintf(stderr, "테스트 파일 범위 오류\n");
		exit(1);
	}
	
	OPT(frameNum, prs, prsNum);
	FIFO(frameNum, prs, prsNum);
	LRU(frameNum, prs, prsNum);
	SecondChance(frameNum, prs, prsNum);

	exit(0);
}

void print_info(char *algoname, int *prs, int prsNum, int frameNum){
	printf("\n\nUsed method : %s\n", algoname);
	printf("page reference string : ");
	for(int i = 0; i < prsNum; i++){
		printf("%d ", prs[i]);
	}
	printf("\n\n\t frame\t");
	for(int i = 1; i<=frameNum; i++){
		printf("%d\t",i);
	}
	printf("page fault\ntime\n");
}

void OPT(int frameNum, int *prs, int prsNum){//Optimal Algorithm 수행
	int *frame;
	int *laterExist;
	int page_fault = 0;//page_fault 발생 횟수
	int changeIdx; //page fault 발생 시 교체할 frame index
	int max = -1;//가장 나중에 나오는 prs의 인덱스
	int swapOut_flag;
	
	
	frame = (int *)malloc(sizeof(int)*(frameNum+1));//마지막 칸의 값이 1이면 page_fault 발생
	laterExist = (int *)malloc(sizeof(int)*frameNum);//page fault 발생 시 언제 다시 언급되는 지 저장 위함
	
	memset(frame, -1, sizeof(int)*(frameNum+1));
	
	print_info("OPT", prs, prsNum, frameNum);
	
	
	for(int p = 0; p < prsNum ; p++){
		printf("%d\t\t", (p+1));//time 부분 출력
		swapOut_flag = 1;//page_fault 발생해서 swapOut 필요시 1
		memset(laterExist, -1, sizeof(int)*frameNum);
		
		frame[frameNum] = 0;
		
		for(int f = 0; f < frameNum; f++){
			if(frame[f] == prs[p]){//hit인 경우
				swapOut_flag = 0;
				break;
			}
			else if(frame[f] == -1){//frame이 비어있는 경우(page_fault 발생)
			//frame개수 만큼만 발생할 것.
				swapOut_flag = 0;
				frame[f] = prs[p]; 
				frame[frameNum] = 1;
				page_fault++;
				break;
			}
		}
		
		if(swapOut_flag){//page_replacement(swap out 할 frame 선택)
		//page_fault 발생했는데 빈 frame이 없는 경우
		
			changeIdx = -1;
			max = -1;
			
			for(int i = prsNum-1; i > p; i--){
				for(int f = 0; f < frameNum; f++){
					if(prs[i] == frame[f]){
						laterExist[f] = i;//다시 참조되는 때의 index 저장
					}
				}
			}
			
			for(int c = 0; c < frameNum ; c++){//page fault시 바꿀 frame idx 선택
				if(laterExist[c] == -1){//후에 다시 참조되는 경우 없음 => swap out
					changeIdx = c;
					break;
				}
				else if(laterExist[c] > max){
					max = laterExist[c];
					changeIdx = c;
				}
			}
			
			frame[changeIdx] = prs[p];
			frame[frameNum] = 1;
			page_fault++;
		}
		
		for(int i = 0; i<=frameNum; i++){
			if(i == frameNum){
				if(frame[i] == 1)
					printf("%c\n", 'F');
				else
					printf("\t\n");
			}
			else{
				if(frame[i] != -1){
					printf("%d\t", frame[i]);
				}
				else
					printf("\t");
			}
		}
	}
	printf("Number of page faults : %d times\n", page_fault);
	
	free(frame);
	free(laterExist);
}

void LRU(int frameNum, int *prs, int prsNum){
	int *frame;
	int *prevExist;
	int page_fault = 0;//page_fault 발생 횟수
	int changeIdx; //page fault 발생 시 교체할 frame index
	int min = -1;//가장 먼저 나오는 prs의 인덱스
	int swapOut_flag;
	
	
	frame = (int *)malloc(sizeof(int)*(frameNum+1));//마지막 칸의 값이 1이면 page_fault 발생
	prevExist = (int *)malloc(sizeof(int)*frameNum);//page fault 발생 시 언제 다시 언급되는 지 저장 위함L
	
	memset(frame, -1, sizeof(int)*(frameNum+1));
	
	print_info("LRU", prs, prsNum, frameNum);
	
	for(int p = 0; p < prsNum ; p++){
		printf("%d\t\t", (p+1));
		swapOut_flag = 1;//page_fault 발생해서 swapOut 필요시 1
		frame[frameNum] = 0;
		memset(prevExist, -1, sizeof(int)*frameNum);
		
		
		for(int f = 0; f < frameNum; f++){
			if(frame[f] == prs[p]){//hit인 경우
				swapOut_flag = 0;
				break;
			}
			else if(frame[f] == -1){//frame이 비어있는 경우(page_fault 발생)
			//frame개수 만큼만 발생할 것.
				swapOut_flag = 0;
				frame[f] = prs[p]; 
				frame[frameNum] = 1;
				page_fault++;
				break;
			}
		}
		
		if(swapOut_flag){//page_replacement(swap out 할 frame 선택)
		//page_fault 발생했는데 빈 frame이 없는 경우
			
			for(int i = 0; i < p ; i++){
				for(int f = 0; f < frameNum; f++){
					if(prs[i] == frame[f]){
						prevExist[f] = i;//과거에 참조되는 때의 index 저장
					}
				}
			}
			
			min = prevExist[0];
			changeIdx = 0;
			for(int c = 0; c < frameNum ; c++){//page fault시 바꿀 frame idx 선택
				if(prevExist[c] < min){
					min = prevExist[c];
					changeIdx = c;
				}
			}
			
			frame[changeIdx] = prs[p];
			frame[frameNum] = 1;
			page_fault++;
		}
		
		for(int i = 0; i<=frameNum; i++){
			if(i == frameNum){
				if(frame[i] == 1)
					printf("%c\n", 'F');
				else
					printf("\t\n");
			}
			else{
				if(frame[i] != -1){
					printf("%d\t", frame[i]);
				}
				else
					printf("\t");
			}
		}
	}
	printf("Number of page faults : %d times\n", page_fault);
	
	free(frame);
	free(prevExist);
	
}

void FIFO(int frameNum, int *prs, int prsNum){//First-in-First-out 알고리즘 수행
	int page_fault = 0;
	int *frame;
	int changeIdx; //다음에 swap out 될 타켓 frame의 idx
	int swapOut_flag;
	
	frame = (int *)malloc(sizeof(int)*(frameNum+1));
	
	memset(frame, -1, sizeof(int)*(frameNum+1));
	changeIdx = 0;
	
	print_info("FIFO", prs, prsNum, frameNum);
	
	for(int p = 0; p<prsNum; p++){
		printf("%d\t\t", (p+1));
		swapOut_flag = 1;//page_fault 발생해서 swapOut 필요시 1
		frame[frameNum] = 0;
		
		for(int f = 0; f < frameNum; f++){
			if(frame[f] == prs[p]){//hit인 경우
				swapOut_flag = 0;
				break;
			}
			else if(frame[f] == -1){//frame이 비어있는 경우(page_fault 발생)
			//frame개수 만큼만 발생할 것.
				swapOut_flag = 0;
				frame[f] = prs[p]; 
				frame[frameNum] = 1;
				page_fault++;
				break;
			}
		}
		
		if(swapOut_flag){
			
			frame[changeIdx] = prs[p];
			frame[frameNum] = 1;
			page_fault++;
			
			if(changeIdx == frameNum-1){
				changeIdx = 0;
			}
			else{
				changeIdx++;
			}
		}
	
		for(int i = 0; i<=frameNum; i++){
			if(i == frameNum){
				if(frame[i] == 1)
					printf("%c\n", 'F');
				else
					printf("\t\n");
			}
			else{
				if(frame[i] != -1){
					printf("%d\t", frame[i]);
				}
				else
					printf("\t");
			}
		}
	}

	printf("Number of page faults : %d times\n", page_fault);
	free(frame);
}

void SecondChance(int frameNum, int *prs, int prsNum){
	int page_fault = 0;
	int *frame;
	int *referencebit;
	int changeIdx; //다음에 swap out 될 타켓 frame의 idx
	int swapOut_flag;
	
	frame = (int *)malloc(sizeof(int)*(frameNum+1));
	referencebit = (int *)malloc(sizeof(int)*frameNum);
	
	memset(frame, -1, sizeof(int)*(frameNum+1));
	memset(referencebit, -1, sizeof(int)*frameNum);
	changeIdx = 0;
	
	print_info("Second-Chance", prs, prsNum, frameNum);
	
	for(int p = 0; p<prsNum; p++){
		printf("%d\t\t", (p+1));
		swapOut_flag = 1;//page_fault 발생해서 swapOut 필요시 1
		frame[frameNum] = 0;
		
		for(int f = 0; f < frameNum; f++){
			if(frame[f] == prs[p]){//hit인 경우
				swapOut_flag = 0;
				referencebit[f]=1;
				break;
			}
			else if(frame[f] == -1){//frame이 비어있는 경우(page_fault 발생)
			//frame개수 만큼만 발생할 것.
				swapOut_flag = 0;
				frame[f] = prs[p]; 
				frame[frameNum] = 1;
				page_fault++;
				referencebit[f] = 0;
				break;
			}
		}
		
		if(swapOut_flag){
			
			while(1){
				if(referencebit[changeIdx] == 1){
					referencebit[changeIdx] = 0;
				}
				else{//swap out
					frame[changeIdx] = prs[p];
					frame[frameNum] = 1;
					page_fault++;
					referencebit[changeIdx] = 0;
					if(changeIdx == (frameNum-1)){
						changeIdx = 0;
					}
					else{
						changeIdx++;
					}
					break;
				}
					
				if(changeIdx == frameNum-1){
					changeIdx = 0;
				}
				else{
					changeIdx++;
				}
			}
		}
	
	
		for(int i = 0; i<=frameNum; i++){
			if(i == frameNum){
				if(frame[i] == 1)
					printf("%c\n", 'F');
				else
					printf("\t\n");
			}
			else{
				if(frame[i] != -1){
					printf("%d\t", frame[i]);
				}
				else
					printf("\t");	
			}
		}
	}

	printf("Number of page faults : %d times\n", page_fault);
	
	free(frame);
	free(referencebit);
}
