// Rijndael.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>

int num_of_rounds = 10;
int block_size = 128; //in bits
int num_of_rows_in_matrix = 4;
int num_of_columns_in_matrix = block_size / 4 / 8;
int ShiftingRowsConstants[4] = { 0,1,2,3 };
uint32_t GF_constant_polynom = 0x11b;


uint8_t SubBytesMatrix[16][16] = {
    {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
    {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
    {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
    {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
    {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
    {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
    {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
    {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
    {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
    {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
    {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
    {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
    {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
    {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
    {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
    {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}
};

uint8_t RoundConstants[4][10] = {
    {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

uint8_t MixColumnMatrix[4][4] = {
    {0x02, 0x03, 0x01, 0x01},
    {0x01, 0x02, 0x03, 0x01},
    {0x01, 0x01, 0x02, 0x03},
    {0x03, 0x01, 0x01, 0x02}
};

void SubBytes(uint8_t** StateMatrix, int n_rows, int n_columns) {
    for (int i = 0; i < n_rows; i++)
    {
        for (int j = 0; j < n_columns; j++)
        {
            uint8_t cur_value = StateMatrix[i][j];
            cur_value = SubBytesMatrix[(cur_value >> 4) % 16][cur_value % 16];
            StateMatrix[i][j] = cur_value;
        }
    }
}

void ShiftRows(uint8_t** StateMatrix) {
    for (int i = 0; i < num_of_rows_in_matrix; i++)
    {
        int shiftpos = ShiftingRowsConstants[i];
        uint8_t* temp_arr = new uint8_t[shiftpos];
        for (int j = 0; j < shiftpos; j++)
            temp_arr[j] = StateMatrix[i][j];

        for (int j = shiftpos; j < num_of_columns_in_matrix; j++)
        {
            StateMatrix[i][j - shiftpos] = StateMatrix[i][j];
        }

        for (int j = num_of_columns_in_matrix - shiftpos; j < num_of_columns_in_matrix; j++)
        {
            StateMatrix[i][j] = temp_arr[j - num_of_columns_in_matrix + shiftpos];
        }
        delete[] temp_arr;
    }
}

void AddRoundKey(uint8_t** StateMatrix, uint8_t** RoundKeyArray, int number_of_round) {
    for (int i = 0; i < num_of_rows_in_matrix; i++)
    {
        for (int j = 0; j < num_of_columns_in_matrix; j++)
        {
            StateMatrix[i][j] = StateMatrix[i][j] ^ RoundKeyArray[i][j + number_of_round * num_of_columns_in_matrix];
        }
    }
}

void GenerateRoundKeys(uint8_t**StartKey, int number_of_rounds, uint8_t**RoundKeyArray) {
    for (int i = 0; i < num_of_rows_in_matrix; i++)
        for (int j = 0; j < num_of_columns_in_matrix; j++)
            RoundKeyArray[i][j] = StartKey[i][j];
    
    for (int i = 1; i <= number_of_rounds; i++)
    {
        std::vector<uint8_t> current_column;
        current_column.resize(0);
        for (int j = 1; j <= num_of_rows_in_matrix; j++)
        {
            uint8_t inttemp;
            uint8_t* temp = &inttemp;
            *temp = RoundKeyArray[j % num_of_rows_in_matrix][i * num_of_columns_in_matrix - 1]; //rotate
            SubBytes(&temp, 1, 1);
            current_column.push_back(*temp);
        }

        for (int j = 0; j < num_of_rows_in_matrix; j++)
            RoundKeyArray[j][i * num_of_columns_in_matrix] = RoundKeyArray[j][(i - 1) * num_of_columns_in_matrix] ^ current_column[j] ^ RoundConstants[j][i - 1];
        //first column of current round key is filled


        for (int j = 1; j < num_of_columns_in_matrix; j++)
            for (int k = 0; k < num_of_rows_in_matrix; k++)
                RoundKeyArray[k][i * num_of_columns_in_matrix + j] = RoundKeyArray[k][(i - 1) * num_of_columns_in_matrix + j] ^ RoundKeyArray[k][i * num_of_columns_in_matrix + j - 1];
     
    }
}

void MixColumns(uint8_t ** StateMatrix) {
    std::vector<uint32_t>temp_column;

    for (int i = 0; i < num_of_columns_in_matrix; i++)
    {
        temp_column.resize(0);
        for (int j = 0; j < num_of_rows_in_matrix; j++)
        {
            uint32_t temp_val = 0;
            for (int k = 0; k < num_of_rows_in_matrix; k++)
            {
                uint32_t stat_mat_k_i_val = StateMatrix[k][i];
                if ((MixColumnMatrix[j][k] == 1) || (MixColumnMatrix[j][k] == 2))
                {
                    temp_val = temp_val ^ (MixColumnMatrix[j][k] * stat_mat_k_i_val);
                }
                else
                {
                    temp_val = temp_val ^ (2 * stat_mat_k_i_val);
                    temp_val = temp_val ^ stat_mat_k_i_val;
                }
            }
            if (temp_val > 0xff)
            {
                temp_val = temp_val ^ GF_constant_polynom;
            }
            temp_column.push_back(temp_val);
        }

        for (int j = 0; j < num_of_rows_in_matrix; j++)
        {
            StateMatrix[j][i] = static_cast<uint8_t>(temp_column[j]);
        }
        
    }
}

int main()
{
    uint8_t** StateMatrix;
    uint8_t** round_key_array;
    uint8_t** StartKey;

    StateMatrix = new uint8_t * [num_of_rows_in_matrix];
    for (int i = 0; i < num_of_rows_in_matrix; i++)
        StateMatrix[i] = new uint8_t[num_of_columns_in_matrix];

    std::cin.unsetf(std::ios::dec); // выключаем ввод в десятичной системе счисления
    std::cin.setf(std::ios::hex);

    std::cout.unsetf(std::ios::dec); // выключаем вывод в десятичной системе счисления
    std::cout.setf(std::ios::hex);

    for (int i = 0; i < num_of_rows_in_matrix; i++)
        for (int j = 0; j < num_of_columns_in_matrix; j++) {
            int temp;
            std::cin >> temp;
            StateMatrix[i][j] = temp;
        }

    StartKey = new uint8_t * [num_of_rows_in_matrix];
    for (int i = 0; i < num_of_rows_in_matrix; i++)
        StartKey[i] = new uint8_t[num_of_columns_in_matrix];

    for (int i = 0; i < num_of_rows_in_matrix; i++)
        for (int j = 0; j < num_of_columns_in_matrix; j++) {
            int temp;
            std::cin >> temp;
            StartKey[i][j] = temp;
        }


    round_key_array = new uint8_t * [num_of_rows_in_matrix];
    for (int i = 0; i < num_of_rows_in_matrix; i++)
        round_key_array[i] = new uint8_t[num_of_columns_in_matrix * (num_of_rounds + 1)];
    


    GenerateRoundKeys(StartKey, num_of_rounds, round_key_array);

    
    AddRoundKey(StateMatrix, round_key_array, 0);

    for (int i = 0; i < num_of_rows_in_matrix; i++)
    {
        for (int j = 0; j < num_of_columns_in_matrix; j++)
        {
            std::cout << int(StateMatrix[i][j]) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';


    for (int i = 1; i <= num_of_rounds - 1; i++)
    {
        SubBytes(StateMatrix, num_of_rows_in_matrix, num_of_columns_in_matrix);
        for (int i = 0; i < num_of_rows_in_matrix; i++)
        {
            for (int j = 0; j < num_of_columns_in_matrix; j++)
            {
                std::cout << int(StateMatrix[i][j]) << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
        ShiftRows(StateMatrix);
        for (int i = 0; i < num_of_rows_in_matrix; i++)
        {
            for (int j = 0; j < num_of_columns_in_matrix; j++)
            {
                std::cout << int(StateMatrix[i][j]) << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
        MixColumns(StateMatrix);
        for (int i = 0; i < num_of_rows_in_matrix; i++)
        {
            for (int j = 0; j < num_of_columns_in_matrix; j++)
            {
                std::cout << int(StateMatrix[i][j]) << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
        AddRoundKey(StateMatrix, round_key_array, i);
        for (int i = 0; i < num_of_rows_in_matrix; i++)
        {
            for (int j = 0; j < num_of_columns_in_matrix; j++)
            {
                std::cout << int(StateMatrix[i][j]) << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }


    SubBytes(StateMatrix, num_of_rows_in_matrix, num_of_columns_in_matrix);
    ShiftRows(StateMatrix);
    AddRoundKey(StateMatrix, round_key_array, num_of_rounds);


    for (int i = 0; i < num_of_rows_in_matrix; i++)
    {
        for (int j = 0; j < num_of_columns_in_matrix; j++)
        {
            std::cout << int(StateMatrix[i][j]) << ' ';
        }
        std::cout << '\n';
    }


    for (int i = 0; i < num_of_rows_in_matrix; i++)
        delete[] StateMatrix[i];
    delete[] StateMatrix;

    for (int i = 0; i < num_of_rows_in_matrix; i++)
        delete[] StartKey[i];
    delete[] StartKey;

    for (int i = 0; i < num_of_rows_in_matrix; i++)
        delete[] round_key_array[i];
    delete[] round_key_array; 
}

