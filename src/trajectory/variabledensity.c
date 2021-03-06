#include "variabledensity.h"

#include "arrayops.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

struct VariableDensity* newVariableDensity()
{
	struct VariableDensity* v = (struct VariableDensity*)malloc(sizeof(struct VariableDensity));

	v->steps = 0;
	v->kcomp = NULL;
	v->compSelect = -1;

	return v;
}

void deleteVariableDensity(struct VariableDensity **variableDensity)
{
	if(*variableDensity)
	{
		free(*variableDensity);
		*variableDensity = NULL;
	}
}

void copyVariableDensity(const struct VariableDensity *from, struct VariableDensity *to)
{
	int s;
	int totalSteps = from->steps;

	for(s=0; s<totalSteps; s++)
		to->step[s] = from->step[s];
	to->steps = from->steps;
	to->compSelect = from->compSelect;
}

void addVariableDensityStep(struct VariableDensity *v, enum VariableDensityFunction function, float kr, float functionParameter, float scale)
{
	int s=0;

    while(s<v->steps && (kr > v->step[s].kr))
		s++;

	if(s==v->steps)
	{
        v->step[s].kr = kr;
		v->step[s].param = functionParameter;
		v->step[s].function = function;
        /*memcpy(&v->fov[v->ndim*s], fov, v->ndim*sizeof(float));*/
        v->step[s].scale = scale;
		v->steps++;
	}
}

void addLinearVariableDensityStep(struct VariableDensity *v, float kr, float scale)
{
	addVariableDensityStep(v, VariableDensityPolynomial, kr, 1, scale);
}

float getFinalScale(const struct VariableDensity *v)
{
//	memcpy(finalFieldOfView, v->fieldOfView, v->numDimensions*sizeof(float));
	return  v->step[v->steps-1].scale;
}


float getScale(const struct VariableDensity *v, float kr)
{
	int s=0;
	float deltaKr;
	float deltaScale;
	float scale = 0;

	while((s<v->steps-1) && (kr >= v->step[s].kr))
		s++;

	if((s==v->steps-1) && (kr >= v->step[s].kr))
	{
		/*for(n=0; n<v->ndim; n++)
			fieldOfView[n] = v->scale[s]*v->fieldOfView[n];*/
		  scale = v->step[s].scale;
	}
	else
	{
		s--;
		deltaKr = v->step[s+1].kr - v->step[s].kr;
		kr -= v->step[s].kr;
		deltaScale = v->step[s+1].scale - v->step[s].scale;

		switch(v->step[s].function)
		{
			case VariableDensityPolynomial:
				if(v->step[s].param>0)
					scale = v->step[s].scale + deltaScale*pow(kr/deltaKr, v->step[s].param);
				else if(v->step[s].param<0)
					scale = v->step[s+1].scale - deltaScale*pow(1.0f-kr/deltaKr, -v->step[s].param);
				else
					scale = 1;
				break;
			default:
				fprintf(stderr, "Error getScale: Invalid vd function %d\n", v->step[s].function);
				break;
		}
	}

//	if(v->compSelect>-1)
//		fieldOfViewScale *= getfieldOfViewCompScale(v, kSpaceRadius);
	return scale;
}

void printVariableDensity(const struct VariableDensity *v)
{
	int s;
	for(s=0; s<v->steps; s++)
		printf("%f\t%f\n", v->step[s].kr, v->step[s].scale);
}

int writeVariableDensity(const char* filename, const struct VariableDensity *v, float krMax, int points)
{
	FILE* file;
	int n;
	float kr;
	float scale;

	file = fopen(filename, "wb");
	if(!file)
	{
	   fprintf(stderr, "Error opening %s for read", filename);
	   return 1;
	}

	for(n=0; n<points; n++)
	{
		kr = n*krMax/points;
		fwrite(&kr, sizeof(float), 1, file);
		scale = getScale(v, kr);
		fwrite(&scale, sizeof(float), 1, file);
	}
	fclose(file);

	return 0;
}

void getFieldOfView(const struct VariableDensity* variableDensity, float kr, const float *fieldOfViewInitial, float *fieldOfViewKr, int dimensions)
{
	float scale = getScale(variableDensity, kr);
	memcpy(fieldOfViewKr, fieldOfViewInitial, dimensions*sizeof(float));
	scalefloats(fieldOfViewKr, dimensions, scale);
}

void getFinalFieldOfView(const struct VariableDensity *variableDensity, const float *initialFieldOfView, float *finalFieldOfView, int dimensions)
{
	float scale = getFinalScale(variableDensity);
	memcpy(finalFieldOfView, initialFieldOfView, dimensions*sizeof(float));
	scalefloats(finalFieldOfView, dimensions, scale);
}


void setToFullySampled(struct VariableDensity *variableDensity)
{
	variableDensity->steps = 0;
	addLinearVariableDensityStep(variableDensity, 0, 1);
	addLinearVariableDensityStep(variableDensity, 1, 1);
}
