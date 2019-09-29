#pragma once
#include "MathHelper.h"
#include "IGZ.h"

namespace IgzModelConverterGUI 
{

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;
	using namespace System::Collections;
	using namespace System::Collections::Generic;

	using namespace System::IO;
	namespace fs = std::experimental::filesystem;
	using namespace msclr::interop;


	struct BoneInfo
	{
		FbxNode* m_pNode;
		//FbxSkeleton* m_pSkeleton;
		FbxCluster* m_pCluster;
	};
	struct EulerAngles
	{
		double roll, pitch, yaw;
	};
	struct Quaternion
	{
		double w, x, y, z;
	};

	EulerAngles ToEulerAngles(Quaternion q)
	{
		EulerAngles angles;

		// roll (x-axis rotation)
		double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
		double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
		angles.roll = atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = +2.0 * (q.w * q.y - q.z * q.x);
		if (fabs(sinp) >= 1)
			angles.pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
		else
			angles.pitch = asin(sinp);

		// yaw (z-axis rotation)
		double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
		double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
		angles.yaw = atan2(siny_cosp, cosy_cosp);

		return angles;
	}
	hkaSkeleton* m_skeleton;
	std::vector<FbxMatrix> matrices;


	// Utility to make sure we always return the right index for the given node
	// Very handy for skeleton hierachy work in the FBX SDK
	FbxNode* GetNodeIndexByName(FbxScene* pScene, std::string NodeName)
	{
		// temp hacky
		FbxNode* NodeToReturn = FbxNode::Create(pScene, "empty");

		for (int i = 0; i < pScene->GetNodeCount(); i++)
		{
			std::string CurrentNodeName = pScene->GetNode(i)->GetName();

			if (CurrentNodeName == NodeName)
			{
				//std::cout << "FOUND BONE AND ITS VALID!!!!!" << "\n";
				NodeToReturn = pScene->GetNode(i);
			}
		}

		return NodeToReturn;
	}

	int GetNodeIDByName(FbxScene* pScene, std::string NodeName)
	{
		int NodeNumber = 0;

		for (int i = 0; i < pScene->GetNodeCount(); i++)
		{
			std::string CurrentNodeName = pScene->GetNode(i)->GetName();

			if (CurrentNodeName == NodeName)
			{
				NodeNumber = i;
			}
		}

		return NodeNumber;
	}
	FbxNode* CreateSkeleton(FbxScene* pScene, const char* pName)
	{
		// get number of bones and apply reference pose
		const int numBones = m_skeleton->GetNumBones();

		// create base limb objects first
		for (int b = 0; b < numBones; b++)
		{
			hkFullBone bone = m_skeleton->GetFullBone(b);
			const hkQTransform* localTransform = m_skeleton->GetBoneTM(b);
			Vector4 pos = localTransform->position;
			Vector4 rot = localTransform->rotation;

			FbxSkeleton* lSkeletonLimbNodeAttribute1 = FbxSkeleton::Create(pScene, pName);

			if ((b == 0))
				lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eRoot);
			else
				lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eLimbNode);

			lSkeletonLimbNodeAttribute1->Size.Set(1.0);
			FbxNode* BaseJoint = FbxNode::Create(pScene, bone.name);
			BaseJoint->SetNodeAttribute(lSkeletonLimbNodeAttribute1);

			// Set Translation
			BaseJoint->LclTranslation.Set(FbxVector4(pos.X, pos.Y, pos.X));
			//BaseJoint->LclTranslation.Set(matrices[b].GetRow(3));

			// convert quat to euler
			FbxQuaternion test (rot.X, rot.Y, rot.Z, rot.W);
			test.Conjugate();
			Quaternion QuatTest = { test.GetAt(3), test.GetAt(0), test.GetAt(1), test.GetAt(2) };
			EulerAngles inAngs = ToEulerAngles(QuatTest);
			//FbxVector4 angs(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z));
			//angs.FixIncorrectValue();
			
			//FbxQuaternion test(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z), rad2deg(inAngs.w));
			//test.Normalize();
			BaseJoint->LclRotation.Set(FbxVector4(rad2deg(inAngs.pitch), rad2deg(inAngs.roll), rad2deg(inAngs.yaw)));
			//BaseJoint->LclRotation.Set(matrices[b].GetRow(1));
			BaseJoint->LclScaling.Set(FbxVector4(11, 11, 11));
			pScene->GetRootNode()->AddChild(BaseJoint);
		}
		// process parenting and transform now
		for (int c = 0; c < numBones; c++)
		{
			int parent = m_skeleton->GetBoneParentID(c);

			if (parent != -1)
			{
				const char* ParentBoneName = m_skeleton->GetFullBone(parent).name;
				const char* CurrentBoneName = m_skeleton->GetFullBone(c).name;
				std::string CurBoneNameString = CurrentBoneName;
				std::string ParentBoneNameString = ParentBoneName;

				FbxNode* ParentJointNode = GetNodeIndexByName(pScene, ParentBoneName);
				FbxNode* CurrentJointNode = GetNodeIndexByName(pScene, CurrentBoneName);
				ParentJointNode->AddChild(CurrentJointNode);
			}
		}

		return pScene->GetRootNode();
	}

	void WritePathFile(char* nstGamePath, char* ctrGamePath, char* skyGamePath)
	{
		FILE* f;
		fopen_s(&f, "GamePaths.dat", "w");
		fprintf_s(f, "%s %s %s", nstGamePath, ctrGamePath, skyGamePath);
	}

	bool ReadPathFile(char* nstGamePath, char* ctrGamePath, char* skyGamePath)
	{
		FILE* f;
		if (fopen_s(&f, "GamePaths.dat", "rb") != 0)
		{
			return false;
		}
		fgets(nstGamePath, sizeof(nstGamePath), f);
		return true;
	}

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{

			int _game = 0;
	private: System::Windows::Forms::Button^  button2;
			 int _type = 0;
			 bool _fileChosen = false;
	private: System::Windows::Forms::RadioButton^  radioButton1;
	private: System::Windows::Forms::RadioButton^  sky;
	private: System::Windows::Forms::ProgressBar^ progressBar1;
			 char* _filename = new char[256];
			 char* _filenamehkx = new char[256];

			 char* _path = new char[256];
	private: System::Windows::Forms::Button^ button3;
	private: System::Windows::Forms::Button^ button4;
	private: System::Windows::Forms::OpenFileDialog^ openFileDialog2;
	private: System::Windows::Forms::TreeView^ treeView1;
	private: System::Windows::Forms::Button^ button5;
	private: System::Windows::Forms::FolderBrowserDialog^ folderBrowserDialog1;
			 
			 int _files = 0;
			 //char* nstGamePath;
			 //char* ctrGamePath;
	private: System::Windows::Forms::CheckBox^ checkBox1;
			 //char* skyGamePath;
			 bool _bones = false;

	public:
		MyForm(void)
		{
			InitializeComponent();
			/*
			if (ReadPathFile(nstGamePath, ctrGamePath, skyGamePath) == false)
			{
				
				//Create a new Path file
				WritePathFile(nstGamePath, ctrGamePath, skyGamePath);
			}
			*/
			//
			//TODO: Add the constructor code here
			//
		}
	public:
		bool ExecuteConvertion(void)
		{
			try
			{
				IGZ::IGZ igZ(_game, _type, _filename);
			}
			catch (System::Exception^ ex)
			{
				MessageBox::Show(ex->Message, "Error",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return false;
			}
			progressBar1->Increment(100 / _files);
			return true;
		}
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::RadioButton^  ActorButton;
	private: System::Windows::Forms::RadioButton^  ModelButton;


	private: System::Windows::Forms::RadioButton^  NSTButton;
	private: System::Windows::Forms::RadioButton^  CTRButton;


	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Panel^  panel2;



	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->ActorButton = (gcnew System::Windows::Forms::RadioButton());
			this->ModelButton = (gcnew System::Windows::Forms::RadioButton());
			this->NSTButton = (gcnew System::Windows::Forms::RadioButton());
			this->CTRButton = (gcnew System::Windows::Forms::RadioButton());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->sky = (gcnew System::Windows::Forms::RadioButton());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->openFileDialog2 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->panel1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Enabled = false;
			this->button1->Location = System::Drawing::Point(655, 9);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(156, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Choose your Igz File Model";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Visible = false;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// ActorButton
			// 
			this->ActorButton->AutoSize = true;
			this->ActorButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ActorButton->ForeColor = System::Drawing::SystemColors::ControlText;
			this->ActorButton->Location = System::Drawing::Point(3, 3);
			this->ActorButton->Name = L"ActorButton";
			this->ActorButton->Size = System::Drawing::Size(55, 17);
			this->ActorButton->TabIndex = 1;
			this->ActorButton->TabStop = true;
			this->ActorButton->Text = L"Actor";
			this->ActorButton->UseVisualStyleBackColor = true;
			this->ActorButton->CheckedChanged += gcnew System::EventHandler(this, &MyForm::ActorButton_CheckedChanged);
			// 
			// ModelButton
			// 
			this->ModelButton->AutoSize = true;
			this->ModelButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ModelButton->Location = System::Drawing::Point(59, 3);
			this->ModelButton->Name = L"ModelButton";
			this->ModelButton->Size = System::Drawing::Size(59, 17);
			this->ModelButton->TabIndex = 2;
			this->ModelButton->TabStop = true;
			this->ModelButton->Text = L"Model";
			this->ModelButton->UseVisualStyleBackColor = true;
			this->ModelButton->CheckedChanged += gcnew System::EventHandler(this, &MyForm::ModelButton_CheckedChanged);
			// 
			// NSTButton
			// 
			this->NSTButton->AutoSize = true;
			this->NSTButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->NSTButton->Location = System::Drawing::Point(3, 3);
			this->NSTButton->Name = L"NSTButton";
			this->NSTButton->Size = System::Drawing::Size(50, 17);
			this->NSTButton->TabIndex = 3;
			this->NSTButton->TabStop = true;
			this->NSTButton->Text = L"NST";
			this->NSTButton->UseVisualStyleBackColor = true;
			this->NSTButton->CheckedChanged += gcnew System::EventHandler(this, &MyForm::NSTButton_CheckedChanged);
			// 
			// CTRButton
			// 
			this->CTRButton->AutoSize = true;
			this->CTRButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->CTRButton->Location = System::Drawing::Point(56, 3);
			this->CTRButton->Name = L"CTRButton";
			this->CTRButton->Size = System::Drawing::Size(50, 17);
			this->CTRButton->TabIndex = 4;
			this->CTRButton->TabStop = true;
			this->CTRButton->Text = L"CTR";
			this->CTRButton->UseVisualStyleBackColor = true;
			this->CTRButton->CheckedChanged += gcnew System::EventHandler(this, &MyForm::CTRButton_CheckedChanged);
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::Transparent;
			this->panel1->Controls->Add(this->radioButton1);
			this->panel1->Controls->Add(this->NSTButton);
			this->panel1->Controls->Add(this->CTRButton);
			this->panel1->Location = System::Drawing::Point(15, 27);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(205, 26);
			this->panel1->TabIndex = 5;
			// 
			// radioButton1
			// 
			this->radioButton1->AutoSize = true;
			this->radioButton1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->radioButton1->Location = System::Drawing::Point(112, 3);
			this->radioButton1->Name = L"radioButton1";
			this->radioButton1->Size = System::Drawing::Size(91, 17);
			this->radioButton1->TabIndex = 5;
			this->radioButton1->TabStop = true;
			this->radioButton1->Text = L"SkyLanders";
			this->radioButton1->UseVisualStyleBackColor = true;
			this->radioButton1->CheckedChanged += gcnew System::EventHandler(this, &MyForm::radioButton1_CheckedChanged);
			// 
			// panel2
			// 
			this->panel2->BackColor = System::Drawing::Color::Transparent;
			this->panel2->Controls->Add(this->sky);
			this->panel2->Controls->Add(this->ActorButton);
			this->panel2->Controls->Add(this->ModelButton);
			this->panel2->Location = System::Drawing::Point(606, 99);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(205, 26);
			this->panel2->TabIndex = 6;
			this->panel2->Visible = false;
			// 
			// sky
			// 
			this->sky->AutoSize = true;
			this->sky->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->sky->Location = System::Drawing::Point(124, 3);
			this->sky->Name = L"sky";
			this->sky->Size = System::Drawing::Size(46, 17);
			this->sky->TabIndex = 3;
			this->sky->TabStop = true;
			this->sky->Text = L"Sky";
			this->sky->UseVisualStyleBackColor = true;
			this->sky->CheckedChanged += gcnew System::EventHandler(this, &MyForm::sky_CheckedChanged);
			// 
			// button2
			// 
			this->button2->Enabled = false;
			this->button2->Location = System::Drawing::Point(684, 41);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 3;
			this->button2->Text = L"Convert!";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Visible = false;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(12, 456);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(799, 26);
			this->progressBar1->TabIndex = 7;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(329, 192);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(113, 23);
			this->button3->TabIndex = 8;
			this->button3->Text = L"Open in Explorer";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &MyForm::Button3_Click);
			// 
			// button4
			// 
			this->button4->Enabled = false;
			this->button4->Location = System::Drawing::Point(618, 70);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(193, 23);
			this->button4->TabIndex = 9;
			this->button4->Text = L"Choose your Havok Animation file";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Visible = false;
			// 
			// openFileDialog2
			// 
			this->openFileDialog2->FileName = L"openFileDialog2";
			// 
			// treeView1
			// 
			this->treeView1->Location = System::Drawing::Point(15, 76);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(282, 374);
			this->treeView1->TabIndex = 10;
			this->treeView1->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &MyForm::TreeView1_AfterSelect);
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(303, 163);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(172, 23);
			this->button5->TabIndex = 11;
			this->button5->Text = L"Choose your game folder";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &MyForm::Button5_Click);
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(499, 167);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(68, 17);
			this->checkBox1->TabIndex = 12;
			this->checkBox1->Text = L"Skeleton";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &MyForm::CheckBox1_CheckedChanged);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->ClientSize = System::Drawing::Size(823, 494);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->treeView1);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->button1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MyForm";
			this->Text = L"IgzModelConverterGUI v1.3";
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		if (_game == 0 || _type == 0)
		{
			MessageBox::Show("Please choose your game and your type", "Error",
				MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}
		if (_type == 1)
		{
			openFileDialog1->FileName = "Select a Igz Model file";
			openFileDialog1->Filter = "Igz Model file (*.igz)|*.igz";
			openFileDialog1->Title = "Open Igz Model File";
			openFileDialog1->Multiselect = false;

			if (openFileDialog1->ShowDialog() == Windows::Forms::DialogResult::OK)
			{
				_fileChosen = true;
				_filename = (char*)(void*)Marshal::StringToHGlobalAnsi(openFileDialog1->FileName);
			}
		}
		else
		{
			openFileDialog1->FileName = "Select a Igz Model file";
			openFileDialog1->Filter = "Igz Model file (*.igz)|*.igz";
			openFileDialog1->Title = "Open Igz Model File";
			openFileDialog1->Multiselect = true;

			if (openFileDialog1->ShowDialog() == Windows::Forms::DialogResult::OK)
			{
				_fileChosen = true;
				_filename = (char*)(void*)Marshal::StringToHGlobalAnsi(openFileDialog1->FileName);
			}
		}
	}
	private: System::Void NSTButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_game = 1;
	}
	private: System::Void CTRButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_game = 2;
	}
	private: System::Void radioButton1_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_game = 3;
	}
	private: System::Void ActorButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_type = 1;
	}
	private: System::Void ModelButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_type = 2;
	}

	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		bool ok = true;
		if (!_fileChosen)
		{
			MessageBox::Show("Please choose your Igz Model file before converting!", "Error",
				MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}
		_files = openFileDialog1->FileNames->Length;
		for(int i = 0; i < _files;i++)
		{
			_filename = (char*)(void*)Marshal::StringToHGlobalAnsi(openFileDialog1->FileNames[i]);
			ok = ExecuteConvertion();
		}
		if (ok == true)
		{
			MessageBox::Show("Finished!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
		progressBar1->Value = 0;
	}

private: System::Void sky_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if (_game == 3)
	{
		_type = 2; //Skylanders works with button 2 Model somehow too... 
	}
	_type = 3;
}
private: System::Void Button3_Click(System::Object^ sender, System::EventArgs^ e) {
	if (!_fileChosen)
	{
		MessageBox::Show("Please choose your game path folder first!", "Error",
			MessageBoxButtons::OK, MessageBoxIcon::Error);
		return;
	}
	if (_filename[0] == -51)
	{
		MessageBox::Show("Please choose your Igz Model file first (you need to convert it)!", "Error",
			MessageBoxButtons::OK, MessageBoxIcon::Error);
		return;
	}
	std::string filename = _filename;
	std::string directory;
	const size_t last_slash_idx = filename.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		directory = filename.substr(0, last_slash_idx);
	}
	ShellExecute(NULL, "open", directory.c_str(), NULL, NULL, SW_SHOWMINIMIZED);
}

private: System::Void TreeView1_AfterSelect(System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e) 
{
	System::String^ test = e->Node->Name;
	std::string fn = msclr::interop::marshal_as<std::string>(test);
	if (fn.substr(fn.find_last_of(".") + 1) == "igz")
	{
		bool ok = true;
		if (e->Node->Name->Contains("actors"))
		{
			if (checkBox1->Checked)
			{
				_bones = true;
			}
			_filenamehkx = GetAnimPath(e->Node->Name);
			_type = 1;
		}
		else if(e->Node->Name->Contains("models"))
		{
			_type = 2;
		}
		else if (e->Node->Name->Contains("sky"))
		{
			_type = 3;
		}
		else
		{
			MessageBox::Show("Unknown Type", "Error",
				MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}
		_files = 1;
		_fileChosen = true;
		_filename = (char*)(void*)Marshal::StringToHGlobalAnsi(e->Node->Name);
		ok = ExecuteConvertion();
		if (ok == true)
		{
			MessageBox::Show("Finished!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
		progressBar1->Value = 0;
	}

}
private: System::Void Button5_Click(System::Object^ sender, System::EventArgs^ e) 
{
	if (folderBrowserDialog1->ShowDialog() == Windows::Forms::DialogResult::OK)
	{
		_fileChosen = true;
		_path = (char*)(void*)Marshal::StringToHGlobalAnsi(folderBrowserDialog1->SelectedPath);
	}
	System::String^ sFileName = gcnew String(_path);
	DirectoryInfo^ directoryInfo = gcnew DirectoryInfo(sFileName);
	BuildTree(directoryInfo, treeView1->Nodes);
}
		 void BuildTree(DirectoryInfo^ directoryInfo, TreeNodeCollection^ addInMe)
		 {
			 TreeNode^ curNode = addInMe->Add(directoryInfo->Name);
			 cli::array< DirectoryInfo^ >^ subDir = directoryInfo->GetDirectories();
			 for (int i = 0; i < subDir->Length; i++)
			 {
				 BuildTree(subDir[i], curNode->Nodes);
			 }
			 cli::array< FileInfo^ >^ subFiles = directoryInfo->GetFiles();

			 for (int i = 0; i < subFiles->Length; i++)
			 {
				 if (subFiles[i]->Extension == gcnew String(".igz"))
				 {
					 curNode->Nodes->Add(subFiles[i]->FullName, subFiles[i]->Name);
				 }
			 }
		 }
		 char* GetAnimPath(System::String^ path)
		 {
			 std::string curPath = msclr::interop::marshal_as<std::string>(path);
			 std::vector<std::string> subdirs;
			 std::string base_filename;
			 std::string root;
			 do
			 {
				 root = curPath.substr(0, curPath.find_last_of("/\\"));
				 base_filename = curPath.substr(curPath.find_last_of("/\\") + 1);
				 if (base_filename == "actors")
				 {
					 break;
				 }
				 subdirs.push_back(base_filename);
				 curPath = curPath.substr(0, curPath.find_last_of("/\\"));
			 } while (base_filename != "actors");
			 std::string hkxPath = root;
			 hkxPath.append("\\anims");
			 std::reverse(subdirs.begin(), subdirs.end());
			 for (size_t i = 0; i < subdirs.size(); i++)
			 {
				 hkxPath.append("\\" + subdirs[i]);
			 }
			 std::string::size_type i = hkxPath.rfind('.', hkxPath.length());
			 std::string newExt = "hka";
			 if (i != std::string::npos) {
				 hkxPath.replace(i + 1, newExt.length(), newExt);
			 }
			 System::String^ test2 = gcnew String(hkxPath.c_str());
			 return (char*)(void*)Marshal::StringToHGlobalAnsi(test2);
		 }
private: System::Void CheckBox1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
}
};
}