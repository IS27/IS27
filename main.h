#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

struct pixel
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
};

#pragma pack(push, 1)
struct head
{
	char type[2];
    unsigned int size;
    unsigned int reserv;
    unsigned int offset;
    unsigned int head_size;
	unsigned int width;
    unsigned int height;
    unsigned int end[7];
};
#pragma pack(pop)

struct pixel **p_in; 
struct pixel **p_out; 
struct head h_bmp;
char *infile;
char *outfile;

void resizev(int );
void resizeh(int );
void rotater();
void rotatel();
int get_pad(int);
void read_file();
void write_file();
void help();

int get_pad(int width)
{
	int i=0;
	while( (width*3 + i) % 4 != 0)
		i++;
	return i;
}

int padding(int width)
{
	int i=0;
	while(width % 4 == 0)
		i++;
	return i;
}

void help()
{
    cout << "\nusage: \n";
    cout << "  chbmp resizev N in_file out_file \n";
    cout << "  chbmp resizeh N in_file out_file \n";
    cout << "  chbmp rotater  in_file out_file \n";
    cout << "  chbmp rotatel  in_file out_file \n";
}

int main(int argc, char** argv)
{
    int pers;

    if(argc < 3)
    {
		help();
		return 1;
    }
  
    if(strcmp(argv[1],"resizev") && strcmp(argv[1],"resizeh") && strcmp(argv[1],"rotater") &&  strcmp(argv[1],"rotatel"))
    {
		help();
		return 1;
    }

    if(!strcmp(argv[1],"resizev"))
    {
		pers = atoi(argv[2]);
		if(argc < 5)
		 {
			 help();
			 return 1;
		 }
		infile=argv[3];
		outfile=argv[4];
		resizev(pers);
    }

    if(!strcmp(argv[1],"resizeh"))
    {
		pers = atoi(argv[2]);
		if(argc < 5)
		{
			 help();
			 return 1;
		}
		infile=argv[3];
		outfile=argv[4];
		resizeh(pers);
    }

    if(!strcmp(argv[1],"rotater"))
    {
		if(argc < 4)
		{
			 help();
			 return 1;
		}
		infile=argv[2];
		outfile=argv[3];
		rotater();
    }

    if(!strcmp(argv[1],"rotatel"))
    {
		if(argc < 4)
		{
			 help();
			 return 1;
		}
		infile=argv[2];
		outfile=argv[3];
		rotatel();
    }
    return 0;
}

void resizev( int pers )
{
	if(pers == 0)
	{
		read_file();
		p_out = p_in;
		write_file();
		return;
	}

	read_file();
	int x = 100/pers;
	int c=0;
	//На сколько увеличится ширина
	for(int i=0; i<h_bmp.height; i++)
		if(i%x == 0)
			c++;

	p_out = new struct pixel*[h_bmp.height+c];

	int m=0, n;
	for(int i=0; i < h_bmp.height; i++)
	{
		n = 0;
		if(i%x == 0)
		{
			if(pers > 0)
			{
				p_out[m] = new struct pixel[h_bmp.width];
				for(int j=0; j < h_bmp.width; j++)
				{
					p_out[m][n] = p_in[i][j];
					n++;
				}
				m++;
				n = 0;
			}

			if(pers < 0)
				continue;
		}

		p_out[m] = new struct pixel[h_bmp.width];

		for(int j=0; j < h_bmp.width; j++)
		{
			p_out[m][n] = p_in[i][j];
			n++;
		}
		m++;
	}
	h_bmp.height = m;
	
	write_file();
}

void resizeh(int pers )
{
	if(pers == 0)
	{
		read_file();
		p_out = p_in;
		write_file();
		return;
	}

	read_file();
	int x = 100/pers;
	int c=0;
	//На сколько увеличится ширина
	for(int i=0; i<h_bmp.width; i++)
		if(i%x == 0)
			c++;

	p_out = new struct pixel*[h_bmp.height];

	int m=0, n;
	for(int i=0; i < h_bmp.height; i++)
	{
		n = 0;
		p_out[m] = new struct pixel[h_bmp.width+c];
		for(int j=0; j < h_bmp.width; j++)
		{
			if(j%x == 0)
			{
				if(pers > 0)
				{
					p_out[m][n] = p_in[i][j];
					n++;
				}
				if(pers < 0)
					continue;
			}
			p_out[m][n] = p_in[i][j];
			n++;
		}
		m++;
	}
	h_bmp.width = n;
	write_file();
}

void rotater()
{
	read_file();
	p_out = new struct pixel*[h_bmp.width];
	for(int i=0; i < h_bmp.width; i++)
		p_out[i] = new struct pixel[h_bmp.height];

	for(int i=0; i < h_bmp.height; i++)
		for(int j=0; j < h_bmp.width; j++)
			p_out[j][i] = p_in[i][j];

	int tmp = h_bmp.height;
	h_bmp.height = h_bmp.width;
	h_bmp.width = tmp;
	write_file();
}

void rotatel()
{
	read_file();
	p_out = new struct pixel*[h_bmp.width];
	for(int i=0; i < h_bmp.width; i++)
		p_out[i] = new struct pixel[h_bmp.height];

	for(int i=0; i < h_bmp.height; i++)
		for(int j=0; j < h_bmp.width; j++)
			p_out[j][i] = p_in[h_bmp.height-i-1][j];

	int tmp = h_bmp.height;
	h_bmp.height = h_bmp.width;
	h_bmp.width = tmp;
	write_file();
}

void read_file()
{
	long int count = 0;
	int i,j,n;
	struct pixel buf[4];
	FILE *fd = fopen(infile, "rb");
	if(!fd)
	{
		perror("open file for read");
		exit(1);
	}
	fread(&h_bmp, sizeof(struct head), 1, fd);
	int pad = get_pad(h_bmp.width);
	p_in = new struct pixel*[h_bmp.height];
	for(i=0; i < h_bmp.height; i++)
	{
		p_in[i] = new struct pixel[h_bmp.width];
		for(j=0; j < h_bmp.width; j++)
		{
			n = fread(&p_in[i][j], 3, 1, fd);
		}
		if(pad != 0)
			fread(buf, pad, 1, fd);
	}
	fclose(fd);
}

void write_file()
{
	int i,j;
	struct pixel buf[4];
	FILE *fd = fopen(outfile, "wb");
	if(!fd)
	{
		perror("open file for write");
		exit(1);
	}
	fwrite(&h_bmp, sizeof(struct head), 1, fd);
	int pad = get_pad(h_bmp.width);
	for(i=0; i < h_bmp.height; i++)
	{
		for(j=0; j < h_bmp.width; j++)
			fwrite(&p_out[i][j], 3, 1, fd);
		if(pad != 0)
			fwrite(buf, pad, 1, fd);
	}
	cout<< "\nbyte write= " << ftell(fd) << endl;
	fclose(fd);
}

