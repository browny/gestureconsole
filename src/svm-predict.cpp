
#include "svm-predict.h"

struct svm_node *x;
int max_nr_attr;

struct svm_model* model;
int predict_probability = 1;   //�ק令�n����v�C�X��

static char *line = NULL;
static int max_line_len;

bool LOADMODELFLAG = false;

//�s��scale�һݪ��Ѽ�
double scale_bottom;
double scale_up;
int scale_para_str_size;
struct scale_para_str
{
	int index;
	double bottom_value;
	double up_value;
};
struct scale_para_str *scale_arr;



static char* readline(FILE *input)
{
	int len;
	
	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}

void exit_with_help()
{
	printf(
	"Usage: svm-predict [options] test_file model_file output_file\n"
	"options:\n"
	"-b probability_estimates: whether to predict probability estimates, 0 or 1 (default 0); for one-class SVM only 0 is supported\n"
	);
	exit(1);
}

//�o�Ө禡Ū����scale�һݪ��Ѽ�
void load_scale_para(const char *scale_file_name,int len){
	
	FILE *fp = fopen(scale_file_name,"rb");

	char cmd[51];

	//��Ū�Ĥ@��
	fscanf(fp,"%50s",cmd);   
	
	//Ū�ĤG��
	fscanf(fp,"%50s",cmd);   
	scale_bottom = atof(cmd);	
	fscanf(fp,"%50s",cmd);   
	scale_up = atof(cmd);

	//Ū�ѤU���Ѽ�
	scale_para_str_size = 0;
	scale_arr = (struct scale_para_str *) malloc(len*sizeof(struct scale_para_str));
	while(fscanf(fp,"%50s",cmd)!=EOF){

		scale_arr[scale_para_str_size].index = atoi(cmd);

		fscanf(fp,"%50s",cmd);
		scale_arr[scale_para_str_size].bottom_value = atof(cmd);

		fscanf(fp,"%50s",cmd);
		scale_arr[scale_para_str_size].up_value = atof(cmd);

		scale_para_str_size++;
	}
	

}

void predict(double *input_feature, int len, double *prob_estimates)
{		

	if(LOADMODELFLAG == false){

		//Ū����scale�һݪ��Ѽ�
		load_scale_para("scale_para",len);		

		//Ū��model
		model = svm_load_model("training_data.txt.scale.model");

		LOADMODELFLAG = true;
	}

	

	//��l�n��feature���Ŷ�
	max_nr_attr = len + 2;
	x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));

	//��featureŪ�ix,�åB��scale���ʧ@
	int i;
	for( i=0 ; i<scale_para_str_size ; i++ ){
		x[i].index = scale_arr[i].index;

		//�o���input��feature���X�åB��scale
		double input_element = input_feature[ x[i].index - 1 ];
		x[i].value = (input_element-scale_arr[i].bottom_value)/(scale_arr[i].up_value-scale_arr[i].bottom_value)*(scale_up-scale_bottom)+scale_bottom;

	}
	x[i].index = -1;

	

	//�������U��class�����v
	svm_predict_probability(model,x,prob_estimates);	

	//
	//free(scale_arr);
	//svm_destroy_model(model);
	free(x);
	

}

void svmUnload()
{
	if(LOADMODELFLAG == true) {
		free(scale_arr);
		svm_destroy_model(model);
	}
	
}