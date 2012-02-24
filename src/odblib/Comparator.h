#ifndef Comparator_H
#define Comparator_H

namespace odb {

const double maxAbsoluteError = 1e-9;
const double maxRelativeError = 1e-9;


class Comparator {
public:
	Comparator(bool checkMissingFlag = true);

	void operator()() { run(); }
	void run(); 

	template <typename T1, typename T2>
		bool compare(T1& it1, const T1& end1, T2& it2, const T2& end2, const string& desc1, const string& desc2);

	void compare(const PathName&, const PathName&);

	void compare(const MetaData&, const MetaData&);
	void compare(int nCols, const double *data1, const double *data2, const MetaData&);

	void checkMissingFlag(bool v) { checkMissingFlag_ = v; }


	inline static double err(double A, double B)
	{
		double relativeError;

		if(fabs(A) <= maxAbsoluteError || fabs(B) <= maxAbsoluteError)
			relativeError = fabs(A-B);
		else if (fabs(B) > fabs(A))
			relativeError = fabs((A - B) / B);
		else
			relativeError = fabs((A - B) / A);
		return relativeError;
	}

	inline static int same(double A,double B) { return err(A,B) < maxRelativeError; }

private:
	long nRow_;
	bool checkMissingFlag_;
	bool NaN_isOK_;
};

template<typename T1, typename T2>
bool Comparator::compare(T1& it1, const T1& end1, T2& it2, const T2& end2, const string& desc1, const string& desc2)
{
	Log::info() << "Comparator::compare: (1) " << desc1 << " to (2) " << desc2 << endl;

	nRow_ = 0;

	compare(it1->columns(), it2->columns());

	for (; it1 != end1 && it2 != end2; ++it1, ++it2)
	{
		++nRow_;

		if (it1->isNewDataset())
			compare(it1->columns(), it2->columns());
		if (it2->isNewDataset())
			compare(it1->columns(), it2->columns());
		//if (it1->isNewDataset() || it2->isNewDataset())
			//compare(it1->columns(), it2->columns());

		compare(it1->columns().size(), it1->data(), it2->data(), it1->columns());
	}

	ASSERT("First file has more rows!"  && ! (it1 != end1));
	ASSERT("Second file has more rows!" && ! (it2 != end2));
	return true; // ?
}

} // namespace odb 

#endif 
