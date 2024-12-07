#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<errno.h>

//constants for field names 
#define MAX_LINE_LENGTH 1024
#define MAX_COUNTIES 3500
#define MAX_FIELD_LENGTH 100
#define MAX_STATE_LENGTH 3

//strucure to hold county data 
typedef struct {
    char county[100];   // character of county names
    char state[MAX_STATE_LENGTH];
    float education[2];
    float ethnicity[10];
    int income_median_household;
    int income_per_capita;
    int income_below_poverty;
    int population;
} County;

// removes double quotes
void remove_quotes(char *str) {
    char *pread = str, *pwrite= str;   // pread: reading char, pwrite: writing result 
    while (*pread) {                   // loops character by character
        if (*pread != '"') {           // " is char
            *pwrite++ = *pread;        // moves write pointer foward after copying
        }
        // moves read pointer to the next character
        pread++;
    }
    // null terminate string to ensure proper string termination
    *pwrite = '\0';
}

//county info for each entry
void display(County *counties, int entries) {
    if (entries == 0) {
        printf("No counties to display.\n");
        return;
    } else {
    for (int i = 0; i < entries; i++){
        printf("County: %s, State: %s\n", counties[i].county, counties[i].state);
        printf("Population: %d\n", counties[i].population);
        printf("Education\n");
        printf("    Bachelor's Degree or Higher: %.2f\n", counties[i].education[0]);
        printf("    High School or Higher: %.2f\n", counties[i].education[1]);
        printf("Ethnicity\n");
        printf("    American Indian and Alaska Native: %.2f", counties[i].ethnicity[0]);
        printf("    Asian Alone: %.6f%%\n", counties[i].ethnicity[1]);
        printf("    Black Alone: %.6f%%\n", counties[i].ethnicity[2]);
        printf("    Hispanic or Latino: %.6f%%\n", counties[i].ethnicity[3]);
        printf("    Native Hawaiian and Other Pacific Islander Alone: %.6f%%\n", counties[1].ethnicity[4]);
        printf("    Two or More Races: %.6f%%\n", counties[i].ethnicity[5]);
        printf("    White Alone: %.6f%%\n", counties[i].ethnicity[6]);
        printf("    White Alone, not Hispanic or Latino: %.6f%%\n", counties[i].ethnicity[7]);
        printf("Income\n");
        printf("    Income Median Household: %d\n", counties[i].income_median_household);
        printf("    Income Per Capita: %d\n", counties[i].income_per_capita);
        printf("    Income.below_poverty: %d\n", counties[i].income_below_poverty);
        printf("\n");
    }
    }
}

// loads county data from CSV file
int load_data(const char *filepath, County **counties){
    FILE *file = fopen(filepath, "r"); // open file for reading
    if (!file) {
        fprintf(stderr, "could not open file '%s'.\n", filepath);
        return 0; // 0 if can't open
    }

    char line[MAX_LINE_LENGTH]; // buffer to hold each line 
    int entries = 0;      // counts the number of counties
    int line_num = 0;           // increments the number of lines that are counted, prints line when necessary
    int allocated_size = 3200;  // allocates memory for counties
    *counties = malloc(allocated_size * sizeof(County)); 

    if (*counties == NULL) {
        fprintf(stderr, "memory allocation failed. \n");
        fclose(file);
        return 0;   // 0 if memory allocation fails 
    }
    
    // read each line from file 
    while (fgets(line, sizeof(line), file)) {
        line_num++;

        County county;
        char *token;
        int field_num = 0;
        int valid_entry = 1; 

        line[strcspn(line, "\n")] = '\0';   // removes newline character at the end of each line if it exists

        // skips header 
        if (line_num == 1) continue;

        token = strtok(line, ","); // splits the lines into tokens by separating with commas
        while (token != NULL) {
            remove_quotes(token);    // remove quotes around the token

            switch(field_num) {
                case 0: //county
                    strncpy(county.county, token, sizeof(county.county)); break;
                case 1: //state
                    strncpy(county.state, token, sizeof(county.state)); break;
                case 5: //education
                    county.education[0] = strtof(token, NULL);break;
                case 6: 
                    county.education[1] = strtof(token, NULL); break;
                case 11: //Ethnicities.American Indian and Alaska Native Alone
                    county.ethnicity[0] = strtof(token, NULL); break;
                case 12: //asian
                    county.ethnicity[1] = strtof(token, NULL); break;
                case 13: //black
                    county.ethnicity[2] = strtof(token, NULL); break;
                case 14: //hispanic or latino
                    county.ethnicity[3] = strtof(token, NULL); break;
                case 15: //Ethnicities.Native Hawaiian and Other Pacific Islander Alone
                    county.ethnicity[4] = strtof(token, NULL); break;
                case 16: //two or more
                    county.ethnicity[5] = strtof(token, NULL); break; 
                case 17: //white alone
                    county.ethnicity[6] = strtof(token, NULL); break;
                case 18: //White Alone not Hispanic or Latino
                    county.ethnicity[7] = strtof(token, NULL); break;
                case 25: // income_median_household
                    county.income_median_household = atoi(token); // converts a string to character 
                    break;
                case 26: // income_per_capita
                    county.income_per_capita = atoi(token); break;
                case 27: // income_below_poverty
                    county.income_below_poverty= atoi(token); break;
                case 38: // population
                    county.population = atoi(token); break;
                default:
                    break;
            }
            field_num++;
            token = strtok(NULL, ","); // gets token from next line
        }
        // if the entry is valid, store it in the counties array 
        if (valid_entry) {
            // if the array is full, resize it
            if (entries >= allocated_size) {
                allocated_size *= 2; //double the size 
                *counties = realloc(*counties, allocated_size * sizeof(County)); //reallocat memory
                if (*counties == NULL) {
                    fprintf(stderr, "memory allocation failed. \n");
                    fclose(file);
                    return 0;   // 0 if failed
                }
            }
            (*counties)[entries++] = county;
        }
    }
    
    fclose(file);
    printf("%d entries loaded\n", entries);
    return entries;
}

// operation to filter the counties only to include those from specified state
void filter_state(County *counties, int *entries, const char *state_abbr) {
    int count = 0;
    for (int i = 0; i < *entries; i++) {
        if (strcmp(counties[i].state, state_abbr) == 0) {
            counties[count++] = counties[i];    // keep matching counties
        }
    }
    *entries = count;
    printf("filter: state == %s (%d entries)\n", state_abbr, *entries);
}

// looks for field
//filters counties based on numeric field, either (greather than or less than) or equal
void filter_field(County *counties, int *entries, const char *line, const char *op, float value) {
    char field[100];  // Declare as mutable char array

    // Use sscanf to extract the field name from the line
    if (sscanf(line, "%99[^:]:%2s:%f", field, (char *)op, &value) == 3) {
        // Validate the operation
        if (strcmp(op, "ge") != 0 && strcmp(op, "le") != 0) {
            fprintf(stderr, "unsupported operation '%s' in filter field.\n", op);
            return;
        }
    }

    int count = 0; // Count how many counties match the filter

    for (int i = 0; i < *entries; i++) {
        float field_value = 0.0;
        int valid_field = 1;

        if (strcmp(field, "Education.Bachelor's Degree or Higher") == 0) {
            field_value = counties[i].education[0];
        } else if (strcmp(field, "Education.High School or Higher") == 0) {
            field_value = counties[i].education[1];
        } else if (strcmp(field, "Ethnicity.American Indian and Alaska Native") == 0) {
            field_value = counties[i].ethnicity[0];
        } else if (strcmp(field, "Ethnicity.Asian Alone") == 0) {
            field_value = counties[i].ethnicity[1];
        } else if (strcmp(field, "Ethnicity.Black Alone") == 0) {
            field_value = counties[i].ethnicity[2];
        } else if (strcmp(field, "Ethnicity.Hispanic or Latino") == 0) {
            field_value = counties[i].ethnicity[3];
        } else if (strcmp(field, "Ethnicity.Native Hawaiian and Other Pacific Islander Alone") == 0) {
            field_value = counties[i].ethnicity[4];
        } else if (strcmp(field, "Ethnicity.Two or More Races") == 0) {
            field_value = counties[i].ethnicity[5];
        } else if (strcmp(field, "Ethnicity.White Alone") == 0) {
            field_value = counties[i].ethnicity[6];
        } else if (strcmp(field, "Ethnicity.White Alone, not Hispanic or Latino") == 0) {
            field_value = counties[i].ethnicity[7];
        } else if (strcmp(field, "Income.Per Capita") == 0) {
            field_value = counties[i].income_per_capita;
        } else if (strcmp(field, "Income.Person Below Poverty") == 0) {
            field_value = counties[i].income_below_poverty;
        } else if (strcmp(field, "Population.") == 0) {
            field_value = counties[i].population;
        } else {
            valid_field = 0;    
        }

        // Comparison
        if (valid_field) {
            if ((strcmp(op, "ge") == 0 && field_value >= value) ||
                (strcmp(op, "le") == 0 && field_value <= value)) {
                counties[count++] = counties[i];
            }
        }
    }
    *entries = count;
    printf("Filter: %s %s %.2f (%d entries)\n", field, op, value, *entries);
    display(counties, *entries);
    
}
// computes total population 
void population_total(County *counties, int entries) {
    int total_population = 0;
    //loops through each county and sum up the population
    for (int i = 0; i < entries; i++) {
        total_population += counties[i].population;

    }
    printf("2014 Population: %d\n", total_population);
}
float calc_pop(County *counties, int entries, const char *field);

//computes total sub population across all entries for a given field 
void population_field(County *counties, int entries, const char *field) {
    float total_subpop = calc_pop(counties, entries, field);
    
    if (total_subpop > 0) {
        printf("2014 %s population: %.0f\n", field, total_subpop);
    } else {
        printf("2014 %s population: %0.0f\n", field, total_subpop);
    }
}

// calculates percentage of the total population with sub-pop specified by field 
void percent_field(County *counties, int entries, const char *field) {
    float total_population = 0.0f;
    float total_subpop = calc_pop(counties, entries, field);

    for (int i = 0; i < entries; i++) {
        total_population += counties[i].population;
    }
    if(total_population > 0) {
        float percentage = (total_subpop / total_population) * 100;
        printf("2014 %s percentage: %.2f%%\n", field, percentage);
    } else {
        printf("2014 %s percentage: 0.00%%\n", field);
    }
}

// Helper function to calculate the total subpopulation for a given field
float calc_pop(County *counties, int entries, const char *field) {
    float total_subpop = 0.0f;

    for (int i = 0; i < entries; i++) {
        if (strcmp(field, "Education.Bachelor's Degree or Higher") == 0) {
            total_subpop += (counties[i].education[0]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Education.High School or Higher") == 0) {
            total_subpop += (counties[i].education[1]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Ethnicities.American Indian and Alaska Native Alone") == 0) {
            total_subpop += (counties[i].ethnicity[0]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Ethnicities.Asian Alone") == 0) {
            total_subpop += (counties[i].ethnicity[1]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Ethnicities.Black Alone") == 0) {
            total_subpop += (counties[i].ethnicity[2]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Ethnicities.Hispanic or Latino") == 0) {
            total_subpop += (counties[i].ethnicity[3]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Ethnicities.Native Hawaiian and Other Pacific Islander Alone") == 0) {
            total_subpop += (counties[i].ethnicity[4]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Ethnicities.Two or More Races") == 0) {
            total_subpop += (counties[i].ethnicity[5]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Ethnicities.White Alone") == 0) {
            total_subpop += (counties[i].ethnicity[6]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Ethnicities.White Alone, not Hispanic or Latino") == 0) {
            total_subpop += (counties[i].ethnicity[7]/100.0f) * counties[i].population;
        } else if (strcmp(field, "Income.Persons Below Poverty Level") == 0) {
            total_subpop += (counties[i].income_below_poverty / 100.0f) * counties[i].population;
        }
    }

    return total_subpop;
}

// Process operations
void process_operations(const char *ops_file, County *counties, int *entries) {
    FILE *file = fopen(ops_file, "r");
    if (!file) {
        fprintf(stderr, "Could not open operations file '%s'.\n", ops_file);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_num = 0;

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        line[strcspn(line, "\n")] = '\0'; // Remove the newline characters

        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }

        char operation[MAX_LINE_LENGTH];
        if (sscanf(line, "%s", operation) != 1) {
            fprintf(stderr, "Operation failed at line %d\n", line_num);
            continue;
        }

        // Process the operation
        if (strcmp(line, "display") == 0) {
            display(counties, *entries);
        } else if (strncmp(operation, "filter-state:", 13) == 0) {
            char state_abbr[3];
            sscanf(line + 13, "%2s", state_abbr);
            filter_state(counties, entries, state_abbr);
        } else if (strncmp(operation, "filter:", 7) == 0) {
            char field[100];  // Declare as mutable char array
            char op[3];                    // Declare as mutable char array
            float value;
            // Use sscanf to extract filter parameters
            if (sscanf(line + 7, "%99[^:]:%2s:%f", field, op, &value) == 3) {
                filter_field(counties, entries, field, op, value);
            } else {
                fprintf(stderr, "Invalid filter format at line %d\n", line_num);
            }
        } else if (strcmp(line, "population-total") == 0) {
            population_total(counties, *entries);
/////////////////////////
        } else if (strncmp(line, "population:", 11) == 0) {
            char field[100];
            sscanf(line + 11, "%99[^:\n]", field);
            population_field(counties, *entries, field);
        } else if (strncmp(line, "percent:", 8) == 0) {
            char field[100];
            sscanf(line + 8, "%99[^\n]", field);
            percent_field(counties, *entries, field);
//////////////////////////
        } else {
            printf("Operation: %s\n", operation);
            fprintf(stderr, "Unsupported operation at line %d\n", line_num);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s <demo_file> <ops_file>\n", argv[0]);
        return 1;   // exit with error code
    }

    // if args are valid, continue
    County *counties = NULL;        // pointer to hold dynamically allocated counties array 
    const char *demo_file = argv[1];
    const char *ops_file = argv[2];

    int entries = load_data(demo_file, &counties); 
    if (entries > 0) {
        printf("Successfully loaded %d entries from the demographics file.\n", entries);
    } else {
        fprintf(stderr, "Failed to load demographic data.\n");
        return 1;
    }
    //process operations on the loaded data
    process_operations(ops_file, counties, &entries);

    //free allocated memory for counties 
    free(counties);

    return 0;
}
