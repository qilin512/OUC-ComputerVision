template<class T>
CMatrixSparse<T>::CMatrixSparse(int rows,int columns) : 
  _rows(rows),_columns(columns)
{
}

template<class T>
CMatrixSparse<T>::CMatrixSparse(const CMatrixSparse & v) 
{ 
  _data.erase(_data.begin(), _data.end());
  _rowEntries.erase(_rowEntries.begin(), _rowEntries.end());

  _rows = v._rows;
  _columns = v._columns;
  _data = v._data;
  _rowEntries = v._rowEntries;
}

template<class T>
T & CMatrixSparse<T>::get(int i,int j)
{
  assert(i>=0 && i<_rows && j>=0 && j<_columns);
  _rowEntries[j].insert[i];
  return _data[pair<int,int>(i,j)];
}

template<class T>
T CMatrixSparse<T>::get(int i,int j) const
{
  assert(i>=0 && i<_rows && j>=0 && j<_columns);
  map<pair<int,int>,double>::const_iterator it =
      _data.find(pair<int,int>(i,j));
  if (it != _data.end())
      return (*it).second;
  else
      return 0;
}


template<class T>
T & CMatrixSparse<T>::operator() (int i,int j)
{
  assert(i>=0 && i<_rows && j>=0 && j<_columns);
  _rowEntries[i].insert(j);
  return _data[pair<int,int>(i,j)];
}

template<class T>
T CMatrixSparse<T>::operator() (int i,int j) const
{
  assert(i>=0 && i<_rows && j>=0 && j<_columns);
  return _data[pair<int,int>(i,j)];
}


template<class T>
CVector<T> CMatrixSparse<T>::operator*(const CVector<T> & v) const
{
  assert(v.Length() == _columns);

  T * data = new T[_rows];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  memset(data,0,_rows*sizeof(T));

  for(typename map<pair<int,int>,T>::const_iterator it=_data.begin();
      it != _data.end(); ++it)
    {
      int i=(*it).first.first;
      int j=(*it).first.second;
      data[i] += (*it).second*v[j];
    }

  return CVector<T>(_rows,data,true);
}

template <class T>
CVector<T>
operator *(const CVector<T> &v, const CMatrixSparse<T> &m)
{
  assert(v.Length() == m.rows());

  T * data = new T[m.columns()];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  memset(data,0,m.columns()*sizeof(T));

  for(typename map<pair<int,int>,T>::const_iterator it=m.data().begin();
      it != m.data().end(); ++it)
    {
      int i=(*it).first.first;
      int j=(*it).first.second;
      data[j] += (*it).second*v[i];
    }

  return CVector<T>(m.columns(),data,true);
}

template<class T>
CMatrixSparse<T>
CMatrixSparse<T>::mTm () const
{
    CMatrixSparse<T> A(_columns,_columns);
    for (map<int,set<int> >::const_iterator rowIt = _rowEntries.begin();
	 rowIt != _rowEntries.end();
	 rowIt++)
    {
	int row = rowIt->first;
	const set<int> &entries = rowIt->second;
	for (set<int>::const_iterator i = entries.begin();
	     i != entries.end(); i++)
	{
	    T x = get(row,*i);
		if (!x) continue;
	    for (set<int>::const_iterator j = entries.begin();
		 j != entries.end(); j++)
	    {
		T y = get(row,*j);
		if (!y) continue;

		A(*i,*j) += x * y;
	    }
	}
    }

    return A;
}

template<class T>
bool CMatrixSparse<T>::isSymmetric() const
{
    if (rows() != columns())
	return false;

    for (typename map<pair<int,int>,T>::const_iterator it=_data.begin();
	 it != _data.end(); ++it)
    {
	int i=(*it).first.first;
	int j=(*it).first.second;
	if ((*it).second != get(j,i))
	{
	    cout << i << " " << j << endl;
	    cout << (*it).second << " " << get(j,i) << endl;
	    return false;
	}

    }

    return true;
}


template <class T>
ostream &
operator << (ostream &os, CMatrixSparse<T> &m)
{
    for (int i = 0; i < m.rows(); i++)
	for (int j = 0; j < m.columns(); j++)
	{
	    T v = m(i,j);
	    if (v != 0)
		os << "(" << i << "," << j << "): " << v << endl;
	}
}

