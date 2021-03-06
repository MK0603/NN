// main.cpp is placed in PUBLIC DOMAIN.
//

#include "stdafx.h"
#include <cmath>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <Matrix.h>


// 入力データと教師データ
struct Data
{
	struct
	{
		double A;	// 入力 a
		double B;	// 入力 b
	} input;
	struct
	{
		double S;	// 出力 s (和)
		double C;	// 出力 c (キャリー)
	} supervisor;
};


int main () {
	// 入力データと教師データ
	const std::vector< Data > datas{
		//	  input         supervisor
		{ { 0.0, 0.0 },{ 0.0, 00.0 } },
	{ { 0.0, 1.0 },{ 1.0, 0.0 } },
	{ { 1.0, 0.0 },{ 1.0, 0.0 } },
	{ { 1.0, 1.0 },{ 0.0, 1.0 } }
	};

	//検算用データ
	const std::vector<Data> test{
		{ { 1.0,1.0 },{ 0.0,1.0 } },
	{ { 1.0,0.0 },{ 1.0,0.0 } }
	};

	//出力ファイル定義
	

	for(int j = 0; j < 100; j++) {
		// 乱数生成器・一様分布器
		std::random_device device;
		std::mt19937_64 generator (device ());
		std::uniform_real_distribution< double > distributor (-8.0, 8.0);

		// 隠れ層への重み
		Matrix< double > weightA (3, 2);
		for(auto i = 0; i < 3; ++i)
		{
			for(auto j = 0; j < 2; ++j)
			{
				weightA (i, j) = distributor (generator);
			}
		}
		// 隠れ層バイアス
		Matrix< double > biasA (3, 1);
		for(auto i = 0; i < 3; ++i)
		{
			biasA (i, 0) = distributor (generator);
		}
		// 出力層への重み
		Matrix< double > weightB (2, 3);
		for(auto i = 0; i < 2; ++i)
		{
			for(auto j = 0; j < 3; ++j)
			{
				weightB (i, j) = distributor (generator);
			}
		}
		// 出力層バイアス
		Matrix< double > biasB (2, 1);
		for(auto i = 0; i < 2; ++i)
		{
			biasB (i, 0) = distributor (generator);
		}

		// 学習回数
		constexpr std::size_t training_times = 100000;
		// 学習率
		constexpr double learning_rate = 2.5;


		//
		// 学習フェーズ
		//
		for(auto i = 0; i < training_times; ++i){
			//誤差初期化
			auto error = 0.0;
			// 誤差に対する隠れ層への重みの偏微分 (∂E/∂WA)
			Matrix< double > dE_dWA (3, 2, 0.0);
			// 誤差に対する隠れ層バイアスの偏微分 (∂E/∂bA)
			Matrix< double > dE_dbA (3, 1, 0.0);
			// 誤差に対する出力層への重みの偏微分 (∂E/∂WB)
			Matrix< double > dE_dWB (2, 3, 0.0);
			// 誤差に対する出力層バイアスの偏微分 (∂E/∂bB)
			Matrix< double > dE_dbB (2, 1, 0.0);

			for(auto & data : datas){
				// 入力層
				Matrix< double > input (2, 1, { data.input.A, data.input.B });

				// 隠れ層を計算する
				auto hidden = weightA * input + biasA;
				hidden (0, 0) = 1.0 / ( 1.0 + std::exp (-hidden (0, 0)) );	//
				hidden (1, 0) = 1.0 / ( 1.0 + std::exp (-hidden (1, 0)) );	//
				hidden (2, 0) = 1.0 / ( 1.0 + std::exp (-hidden (2, 0)) );	// 活性化関数:シグモイド関数

				// 出力層を計算する
				auto output = weightB * hidden + biasB;
				output (0, 0) = 1.0 / ( 1.0 + std::exp (-output (0, 0)) );	//
				output (1, 0) = 1.0 / ( 1.0 + std::exp (-output (1, 0)) );	// 活性化関数:シグモイド関数

				// 誤差
				error += 0.5 * ( std::pow (output (0, 0) - data.supervisor.S, 2.0) + std::pow (output (1, 0) - data.supervisor.C, 2.0) );

				// ∂E/∂bB
				Matrix< double > supervisor (2, 1, { data.supervisor.S, data.supervisor.C });
				auto temp0 = output - supervisor;
				auto temp1 = temp0.element_product (output).element_product (Matrix< double > (2, 1, 1.0) - output);
				dE_dbB += temp1;

				// ∂E/∂WB
				auto temp2 = temp1 * hidden.transpose ();
				dE_dWB += temp2;

				// ∂E/∂bA
				auto temp3 = ( temp1.transpose () * weightB ).transpose ();
				auto temp4 = temp3.element_product (hidden).element_product (Matrix< double > (3, 1, 1.0) - hidden);
				dE_dbA += temp4;

				// ∂E/∂WA
				auto temp5 = temp4 * input.transpose ();
				dE_dWA += temp5;
			}

			// 100 回ごとに報告
			//if ( i % 100 == 0 )
			//{
				//std::cout << i << " -- " << error << std::endl;
			//}

			// 重みとバイアスに反映
			weightA -= ( dE_dWA *= learning_rate );
			biasA -= ( dE_dbA *= learning_rate );
			weightB -= ( dE_dWB *= learning_rate );
			biasB -= ( dE_dbB *= learning_rate );
		}

		//
		// 結果 (学習フェーズとほぼ同じコードなのでコメントは省略)
		//

		/*auto error = 0.0;
		for(auto data : datas){
			Matrix< double > input (2, 1, { data.input.A, data.input.B });
			auto hidden = weightA * input + biasA;
			hidden (0, 0) = 1.0 / ( 1.0 + std::exp (-hidden (0, 0)) );
			hidden (1, 0) = 1.0 / ( 1.0 + std::exp (-hidden (1, 0)) );
			hidden (2, 0) = 1.0 / ( 1.0 + std::exp (-hidden (2, 0)) );
			auto output = weightB * hidden + biasB;
			output (0, 0) = 1.0 / ( 1.0 + std::exp (-output (0, 0)) );
			output (1, 0) = 1.0 / ( 1.0 + std::exp (-output (1, 0)) );
			error += 0.5 * ( std::pow (output (0, 0) - data.supervisor.S, 2.0) + std::pow (output (1, 0) - data.supervisor.C, 2.0) );
		}*/

		//outputfile << training_times << "," << error << ","
		//	<< weightA (0, 0) << "," << weightA (1, 0) << "," << weightA (2, 0) << ","
		//	<< weightA (0, 1) << "," << weightA (1, 1) << "," << weightA (2, 1) << ","
		//	<< weightB (0, 0) << "," << weightB (1, 0) << ","
		//	<< weightB (0, 1) << "," << weightB (1, 1) << ","
		//	<< weightB (0, 2) << "," << weightB (1, 2) << ","
		//	<< biasA (0, 0) << "," << biasA (1, 0) << "," << biasA (2, 0) << ","
		//	<< biasB (0, 0) << "," << biasB (1, 0) << "\r\n";
		std::cout << "周回 : " << j << std::endl;



		
		for(auto data : test) {
			Matrix<double>testInput (2, 1, { data.input.A,data.input.B });
			auto hidden = weightA * testInput + biasA;
			hidden (0, 0) = 1.0 / ( 1.0 + std::exp (-hidden (0, 0)) );
			hidden (1, 0) = 1.0 / ( 1.0 + std::exp (-hidden (1, 0)) );
			hidden (2, 0) = 1.0 / ( 1.0 + std::exp (-hidden (2, 0)) );
			auto trialOutput = weightB * hidden + biasB;
			trialOutput (0, 0) = 1.0 / ( 1.0 + std::exp (-trialOutput (0, 0)) );
			trialOutput (1, 0) = 1.0 / ( 1.0 + std::exp (-trialOutput (1, 0)) );
			std::ofstream outputfile ("C:/Users/Fate/Desktop/SumProduct.txt", std::ios::app);
			outputfile << data.supervisor.S << "," << trialOutput (0, 0) <<","<< data.supervisor.C<< "," << trialOutput (1, 0) << "\r\n";
			outputfile.close ();
		}
	}
	
	return 0;
}