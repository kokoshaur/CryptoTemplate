#pragma once

#include <algorithm>

#include "Resampler.cpp"
#include <math.h>

#define SIZESHOW 2
#define COUTANALIZBLOCKS 1
#define COUTSHOWBLOCKS 1

class Crypto {
private:
	unsigned const int W = 64;
	int r = 0;
	Resampler* file;
	vector<int> L;
	vector<char> Iv;
	vector<char> nextIv;
	vector<char> S;
	bool isCrypto;
	int log_w = log2(W);
	int long long modul = pow(2, W);

	int coutAnalizBlocks = 0;
	int coutShowBlocks = 0;
	
	vector<int> coutSwap;

public:
	Crypto(Resampler* resampler, vector<char>& key, vector<char>& IV, int R = -1)
	{
		file = resampler;
		Iv = IV;
		if (R < 0)
			r = key.size() / 16;
		else
			r = R;
		obsKey(key);
		coutSwap.resize(SIZESHOW * 8);
	}

	void mod(bool isCrypto)
	{
		this->isCrypto = isCrypto;
		file->mod(isCrypto);
		nextIv = Iv;
	}

	void start()
	{
		vector<char> block = file->nextBlock();
		while (block.size() != 0) {
			cryptoBlock(block);
			file->setBlock(block);
			block = file->nextBlock();
		}
	}

private:
	void obsKey(vector<char>& key)
	{
		//Расширение ключа
	}

	void cryptoBlock(vector<char>& block)
	{
		if (isCrypto) {
			CBC(block);
			nextIv = block;
			forceKey(block);
			analiz(block);
		}
		else {
			forceKey(block);//Если расшифрование происходит не так же как шифрование, то написать свою функцию расшифрования
			vector<char> buf = block;
			CBC(block);
			nextIv = buf;
		}
	}

	void forceKey(vector<char>& block)
	{
		//Шифрование
	}

	void CBC(vector<char>& block)
	{

		for (int i = 0; i < block.size(); i++)
			if (isCrypto)
				block[i] ^= nextIv[i];
			else {
				char tmp = nextIv[i] ^ block[i];
				block[i] = tmp;
			}

	}

	char leftShift(char arg, char n) {
		n <<= W - log_w;
		n >>= W - log_w;
		return (arg << n) | (arg >> (W - n));
	}

	char rightShift(char arg, char n) {
		n >>= W - log_w;
		n <<= W - log_w;
		return (arg >> n) | (arg << (W - n));
	}

	void analiz(vector<char> block)
	{
		if (coutAnalizBlocks > COUTANALIZBLOCKS)
			return;
		
		avtocor(block);

		cout << endl << "тесть серий" << endl;
		series(block);

		cout << endl << "частотный тест" << endl;
		freq(block);

		coutAnalizBlocks++;
	}

	void show(vector<char> oldBlock, vector<vector<char>> block)
	{
		showBlock(oldBlock, block[0]);
		for (int i = 1; i < r; i++)
		{
			showBlock(block[i - 1], block[i]);
		}
	}

	void showBlock(vector<char> oldBlock, vector<char> block)
	{
		char subj;
		for (int i = 0; i < SIZESHOW; i++)
		{
			subj = oldBlock[i];
			for (int j = 0; j < 8; j++)
			{
				printf("%c", (subj & 0x80) ? '1' : '0');
				subj <<= 1;
				printf(" ");
			}
		}
		printf("\n");
		for (int i = 0; i < SIZESHOW; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((block[i] & 0x80) == (oldBlock[i] & 0x80))
					printf("%c", (block[i] & 0x80) ? '1' : '0');
				else
				{
					cout << "\x1b[31m" << ((block[i] & 0x80) ? '1' : '0') << "\x1b[0m";
					coutSwap[i * 8 + j]++;
				}

				block[i] <<= 1; oldBlock[i] <<= 1;

				printf(" ");
			}
		}
		printf("\n");
		printf("\n");
	}

	void avtocor(vector<char> block)
	{
		vector<pair<double, int>> list(block.size());

		for (int i = 0; i < block.size(); i++)
		{
			double werh = 0, niz = 0;
			for (int k = 0; k < r; k++)
				werh += (norm(block[k]) * norm(block[(int)((k + i) % r)]));

			for (int k = 0; k < r; k++)
				niz += (norm(block[k]) * norm(block[k]));

			double q = (double)(werh / niz);
			list[i].first = q;
			list[i].second = i;
		}
	}

	char norm(char subj)
	{
		return (char)(subj * 2 - 1);
	}

	void series(vector<char> block)
	{
		int mas[7] = { 0 };

		int Bit = (block[0] & 1);
		int serial = 1;
		for (int j = 0; j < block.size(); j++)
		{
			char buf = block[j];
			int i = 0;
			for (; i < 8; i++, buf >>= 1)
				if ((buf & 1) == Bit)
					serial++;
				else
				{
					if (serial > 6)
						serial = 6;

					mas[serial]++;
					Bit = (buf & 1);
					serial = 1;
				}
		}

		for (int i = 1; i < 7; i++)
		{
			cout << i << ": " << mas[i] << endl;
		}
	}

	void freq(vector<char> block)
	{
		int numOfNum = 0;
		for (int i = 0; i < block.size(); i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((block[i] & 0x80))
					numOfNum++;
				block[i] <<= 1;
			}
		}

		cout << "Количество нулей: " << block.size() * 8 - numOfNum << endl << "Количество единиц: " << numOfNum << endl;
	}
};