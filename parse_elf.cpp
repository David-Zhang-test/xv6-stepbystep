#include <iostream>
#include <iomanip>
#include <fstream>
// #include "initcode.h"
using namespace std;

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

#define ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian

// File header
struct elfhdr
{
	uint magic; // must equal ELF_MAGIC
	uchar elf[12];
	ushort type;
	ushort machine;
	uint version;
	uint64 entry;
	uint64 phoff;
	uint64 shoff;
	uint flags;
	ushort ehsize;
	ushort phentsize;
	ushort phnum;
	ushort shentsize;
	ushort shnum;
	ushort shstrndx;
};

// Program section header
struct proghdr
{
	uint32 type;
	uint32 flags;
	uint64 off;
	uint64 vaddr;
	uint64 paddr;
	uint64 filesz;
	uint64 memsz;
	uint64 align;
};

// Values for Proghdr type
#define ELF_PROG_LOAD 1

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC 1
#define ELF_PROG_FLAG_WRITE 2
#define ELF_PROG_FLAG_READ 4

int main()
{
	ifstream is("./initcode.out", ios_base::in | ios_base::binary);
	is.seekg(0, ios::end);
	int fileSize = is.tellg();
	cout << "File size: " << fileSize << "B" << endl;
	unsigned char *initcode_out = new unsigned char[fileSize];
	is.seekg(0, ios::beg);
	is.read((char *)initcode_out, fileSize);

	ofstream os("./initcode.txt");
	void *p = initcode_out;
	elfhdr *fHdr = (elfhdr *)p;
	if (fHdr->magic != 0x464C457F || fHdr->type != 2 || fHdr->machine != 0xf3)
	{
		cout << "File format error!" << endl;
		return 0;
	}
	os << "ELF Header:\n"
	   << "\tmagic: 0x" << std::hex << fHdr->magic << "\n"
	   << "\telf[12]: " << "[ignore]\n"
	   << "\ttype: 0x" << std::hex << fHdr->type << "\n"
	   << "\tmachine: 0x" << std::hex << fHdr->machine << "\n"
	   << "\tversion: 0x" << std::hex << fHdr->version << "\n"
	   << "\tentry: 0x" << std::hex << fHdr->entry << "\n"
	   << "\tphoff: 0x" << std::hex << fHdr->phoff << "\n"
	   << "\tshoff: 0x" << std::hex << fHdr->shoff << "\n"
	   << "\tflags: 0x" << std::hex << fHdr->flags << "\n"
	   << "\tehsize: 0x" << std::hex << fHdr->ehsize << "\n"
	   << "\tphentsize: 0x" << std::hex << fHdr->phentsize << "\n"
	   << "\tphnum: 0x" << std::hex << fHdr->phnum << "\n"
	   << "\tshentsize: 0x" << std::hex << fHdr->shentsize << "\n"
	   << "\tshnum: 0x" << std::hex << fHdr->shnum << "\n"
	   << "\tshstrndx: 0x" << std::hex << fHdr->shstrndx << "\n"
	   << endl;

	proghdr *pHdr = (proghdr *)((uint64)initcode_out + fHdr->phoff);
	for (int i = 0; i < fHdr->phnum; i++)
	{
		os << "Program section header " << i << ":\n"
		   << "\ttype: 0x" << std::hex << pHdr->type << "\n"
		   << "\tflags: 0x" << std::hex << pHdr->flags << "\n"
		   << "\toff: 0x" << std::hex << pHdr->off << "\n"
		   << "\tvaddr: 0x" << std::hex << pHdr->vaddr << "\n"
		   << "\tpaddr: 0x" << std::hex << pHdr->paddr << "\n"
		   << "\tfilesz: 0x" << std::hex << pHdr->filesz << "\n"
		   << "\tmemsz: 0x" << std::hex << pHdr->memsz << "\n"
		   << "\talign: 0x" << std::hex << pHdr->align << "\n"
		   << endl;
		os << "\tContent:";
		if (pHdr->type != ELF_PROG_LOAD)
			os << "\n\t[Ignore: section type not ELF_PROG_LOAD]";
		else if (pHdr->filesz == 0)
			os << "\n\t[None: 0 size]";
		else
		{
			unsigned char *p = (unsigned char *)((uint64)initcode_out + pHdr->off);
			for (int j = 0; j < pHdr->filesz; j++)
			{
				if (j % 12 == 0)
					os << "\n\t";
				os << "0x" << std::hex << std::setw(2) << std::setfill('0') << (uint)(*p) << ", ";
				p++;
			}
		}
		os << endl
		   << endl;
		pHdr += 1;
	}
	os.close();
	cout << "Finish!" << endl;
	return 0;
}