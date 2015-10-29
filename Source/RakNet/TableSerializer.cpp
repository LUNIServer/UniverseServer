#include "TableSerializer.h"
#include "DS_Table.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "RakAssert.h"

void TableSerializer::SerializeTable(DataStructures::Table *in, RakNet::BitStream *out)
{
	DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> *cur = in->GetRows().GetListHead();
	DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns=in->GetColumns();
	SerializeColumns(in, out);
	out->Write((unsigned)in->GetRows().Size());
	unsigned rowIndex;
	while (cur)
	{
		for (rowIndex=0; rowIndex < (unsigned)cur->size; rowIndex++)
		{
			SerializeRow(cur->data[rowIndex], cur->keys[rowIndex], columns, out);
		}
		cur=cur->next;
	}
}
void TableSerializer::SerializeColumns(DataStructures::Table *in, RakNet::BitStream *out)
{
	DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns=in->GetColumns();
	out->Write((unsigned)columns.Size());
	unsigned i;
	for (i=0; i<columns.Size(); i++)
	{
		stringCompressor->EncodeString(columns[i].columnName, _TABLE_MAX_COLUMN_NAME_LENGTH, out);
		out->Write((unsigned char)columns[i].columnType);
	}
}
void TableSerializer::SerializeColumns(DataStructures::Table *in, RakNet::BitStream *out, DataStructures::List<int> &skipColumnIndices)
{
	DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns=in->GetColumns();
	out->Write((unsigned)columns.Size()-skipColumnIndices.Size());
	unsigned i;
	for (i=0; i<columns.Size(); i++)
	{
		if (skipColumnIndices.GetIndexOf(i)==(unsigned)-1)
		{
			stringCompressor->EncodeString(columns[i].columnName, _TABLE_MAX_COLUMN_NAME_LENGTH, out);
			out->Write((unsigned char)columns[i].columnType);
		}		
	}
}
bool TableSerializer::DeserializeTable(unsigned char *serializedTable, unsigned int dataLength, DataStructures::Table *out)
{
	RakNet::BitStream in((unsigned char*) serializedTable, dataLength, false);
	return DeserializeTable(&in, out);
}
bool TableSerializer::DeserializeTable(RakNet::BitStream *in, DataStructures::Table *out)
{
	unsigned rowSize;
	DeserializeColumns(in,out);
	if (in->Read(rowSize)==false || rowSize>100000)
		return false; // Hacker crash prevention

	unsigned rowIndex;
	for (rowIndex=0; rowIndex < rowSize; rowIndex++)
	{
		if (DeserializeRow(in, out)==false)
			return false;
	}
	return true;
}
bool TableSerializer::DeserializeColumns(RakNet::BitStream *in, DataStructures::Table *out)
{
	unsigned columnSize;
	unsigned char columnType;
	char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH];
	if (in->Read(columnSize)==false || columnSize > 10000)
		return false; // Hacker crash prevention

	out->Clear();
	unsigned i;
	for (i=0; i<columnSize; i++)
	{
		stringCompressor->DecodeString(columnName, 32, in);
		in->Read(columnType);
		out->AddColumn(columnName, (DataStructures::Table::ColumnType)columnType);
	}
	return true;
}
void TableSerializer::SerializeRow(DataStructures::Table::Row *in, unsigned keyIn, DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns, RakNet::BitStream *out)
{
	unsigned cellIndex;
	out->Write(keyIn);
	for (cellIndex=0; cellIndex<columns.Size(); cellIndex++)
	{
		SerializeCell(out, in->cells[cellIndex], columns[cellIndex].columnType);
	}
}
void TableSerializer::SerializeRow(DataStructures::Table::Row *in, unsigned keyIn, DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns, RakNet::BitStream *out, DataStructures::List<int> &skipColumnIndices)
{
	unsigned cellIndex;
	out->Write(keyIn);
	for (cellIndex=0; cellIndex<columns.Size(); cellIndex++)
	{
		if (skipColumnIndices.GetIndexOf(cellIndex)==(unsigned)-1)
		{
			SerializeCell(out, in->cells[cellIndex], columns[cellIndex].columnType);
		}
	}
}
bool TableSerializer::DeserializeRow(RakNet::BitStream *in, DataStructures::Table *out)
{
	DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns=out->GetColumns();
	unsigned cellIndex;
	DataStructures::Table::Row *row;
	unsigned key;
	if (in->Read(key)==false)
		return false;
	row=out->AddRow(key);
	for (cellIndex=0; cellIndex<columns.Size(); cellIndex++)
	{
		if (DeserializeCell(in, row->cells[cellIndex], columns[cellIndex].columnType)==false)
		{
			out->RemoveRow(key);
			return false;
		}
	}
	return true;
}
void TableSerializer::SerializeCell(RakNet::BitStream *out, DataStructures::Table::Cell *cell, DataStructures::Table::ColumnType columnType)
{
	out->Write(cell->isEmpty);
	if (cell->isEmpty==false)
	{
		if (columnType==DataStructures::Table::NUMERIC)
		{
			out->Write(cell->i);
		}
		else if (columnType==DataStructures::Table::STRING)
		{
			stringCompressor->EncodeString(cell->c, 65535, out);
		}
		else if (columnType==DataStructures::Table::POINTER)
		{
			out->Write(cell->ptr);
		}
		else
		{
			// Binary
			assert(columnType==DataStructures::Table::BINARY);
			RakAssert(cell->i>0);
			out->Write((unsigned)cell->i);
			out->WriteAlignedBytes((const unsigned char*) cell->c, cell->i);
		}
	}
}
bool TableSerializer::DeserializeCell(RakNet::BitStream *in, DataStructures::Table::Cell *cell, DataStructures::Table::ColumnType columnType)
{
	bool isEmpty;
	int value;
	void *ptr;
	char tempString[65535];
	cell->Clear();

	if (in->Read(isEmpty)==false)
		return false;
	if (isEmpty==false)
	{
		if (columnType==DataStructures::Table::NUMERIC)
		{
			if (in->Read(value)==false)
				return false;
			cell->Set(value);
		}
		else if (columnType==DataStructures::Table::STRING)
		{
			if (stringCompressor->DecodeString(tempString, 65535, in)==false)
				return false;
			cell->Set(tempString);
		}
		else if (columnType==DataStructures::Table::POINTER)
		{
			if (in->Read(ptr)==false)
				return false;
			cell->SetPtr(ptr);
		}
		else
		{
			// Binary
			assert(columnType==DataStructures::Table::BINARY);
			if (in->Read(value)==false || value > 10000000)
				return false; // Sanity check to max binary cell of 10 megabytes
			in->AlignReadToByteBoundary();
			if (BITS_TO_BYTES(in->GetNumberOfUnreadBits())<(BitSize_t)value)
				return false;
			cell->Set((char*) in->GetData()+BITS_TO_BYTES(in->GetReadOffset()), value);
			in->IgnoreBits(BYTES_TO_BITS(value));
		}
	}
	return true;
}
void TableSerializer::SerializeFilterQuery(RakNet::BitStream *in, DataStructures::Table::FilterQuery *query)
{
	stringCompressor->EncodeString(query->columnName,_TABLE_MAX_COLUMN_NAME_LENGTH,in,0);
	in->WriteCompressed(query->columnIndex);
	in->Write((unsigned char) query->operation);
	in->Write(query->cellValue->isEmpty);
	if (query->cellValue->isEmpty==false)
	{
		in->WriteAlignedBytesSafe((const char*)query->cellValue->c,query->cellValue->i,10000000); // Sanity check to max binary cell of 10 megabytes
		in->Write(query->cellValue->i);
		in->Write(query->cellValue->ptr);

	}
}
bool TableSerializer::DeserializeFilterQuery(RakNet::BitStream *out, DataStructures::Table::FilterQuery *query)
{
	bool b;
	RakAssert(query->cellValue);
	stringCompressor->DecodeString(query->columnName,_TABLE_MAX_COLUMN_NAME_LENGTH,out,0);
	out->ReadCompressed(query->columnIndex);
	unsigned char op;
	out->Read(op);
	query->operation=(DataStructures::Table::FilterQueryType) op;
	query->cellValue->Clear();
	b=out->Read(query->cellValue->isEmpty);
	if (query->cellValue->isEmpty==false)
	{
		// HACK - cellValue->i is used for integer, character, and binary data. However, for character and binary c will be 0. So use that to determine if the data was integer or not.
		BitSize_t inputLength;
		out->ReadAlignedBytesSafeAlloc(&query->cellValue->c,inputLength,10000000); // Sanity check to max binary cell of 10 megabytes
		query->cellValue->i=(int) inputLength;
		if (query->cellValue->c==0)
			out->Read(query->cellValue->i);
		else
			out->IgnoreBytes(sizeof(query->cellValue->i));
		b=out->Read(query->cellValue->ptr);
	}
	return b;
}
void TableSerializer::SerializeFilterQueryList(RakNet::BitStream *in, DataStructures::Table::FilterQuery *query, unsigned int numQueries, unsigned int maxQueries)
{
	in->Write((bool)(query && numQueries>0));
	if (query==0 || numQueries<=0)
		return;

	RakAssert(numQueries<=maxQueries);
	in->WriteCompressed(numQueries);
	unsigned i;
	for (i=0; i < numQueries; i++)
	{
		SerializeFilterQuery(in, query);
	}
}
bool TableSerializer::DeserializeFilterQueryList(RakNet::BitStream *out, DataStructures::Table::FilterQuery **query, unsigned int *numQueries, unsigned int maxQueries, int allocateExtraQueries)
{
	bool b, anyQueries=false;
	out->Read(anyQueries);
	if (anyQueries==false)
	{
		if (allocateExtraQueries<=0)
			*query=0;
		else
			*query=new DataStructures::Table::FilterQuery[allocateExtraQueries];

		*numQueries=0;
		return true;
	}
	b=out->ReadCompressed(*numQueries);
	if (*numQueries>maxQueries)
	{
		RakAssert(0);
		*numQueries=maxQueries;
	}
	if (*numQueries==0)
		return b;

	*query=new DataStructures::Table::FilterQuery[*numQueries+allocateExtraQueries];
	DataStructures::Table::FilterQuery *queryPtr = *query;

	unsigned i;
	for (i=0; i < *numQueries; i++)
	{
		queryPtr[i].cellValue=new DataStructures::Table::Cell;
		b=DeserializeFilterQuery(out, queryPtr+i);
	}

	return b;
}
void TableSerializer::DeallocateQueryList(DataStructures::Table::FilterQuery *query, unsigned int numQueries)
{
	if (query==0 || numQueries==0)
		return;

	unsigned i;
	for (i=0; i < numQueries; i++)
		delete query[i].cellValue;
	delete [] query;
}
