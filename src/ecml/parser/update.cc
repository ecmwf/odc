
void Cell::update(const std::string& keyword, Cell* v) 
{
    //showGraph(string("value: request = ") + str() + ", keyword = " + keyword + ", v = " + v->str());

    const std::string k (StringTools::lower(keyword));

    Cell* lastMatch (this); 

    for (Cell* r (this); r; r = r->rest())
    {
        if (StringTools::lower(r->text()) == k)
            lastMatch = r;

        if (! r->rest())
        {
            r->rest(new Cell("", keyword, v, 0));
            return; 
        }
    }

    delete lastMatch->value();
    lastMatch->value(v);
}
