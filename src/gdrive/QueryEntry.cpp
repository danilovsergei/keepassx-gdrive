#include "QueryEntry.h"

bool QueryEntry::filtersInitialized=false;
QMap<QueryEntry::QueryFilter,QString> QueryEntry::filters=QMap<QueryEntry::QueryFilter,QString>();


/**
 * @brief QueryEntry::getFilters - use to store filter templates for different google drive parameters
 * @return - returns map of the filter templates
 */
const QMap<QueryEntry::QueryFilter, QString>& QueryEntry::getFilters() {
if (!QueryEntry::filtersInitialized) {
QueryEntry::filters[QueryFilter::DBNAME]="title = '%1'";
QueryEntry::filters[QueryFilter::LAST_MODIFIED]=QString("properties has { key='%1' and value='%2' and visibility='PUBLIC'}").arg(LAST_MODIFIED);
}
return QueryEntry::filters;
}


QueryEntry::QueryEntry(const QueryFilter& entryType,const QStringList& parameters)
   :type(entryType),parameters(parameters)
{

}

/**
 * @brief QueryEntry::getEntries - used to construct filter query for google drive
 * @param entries - input list of entries. Eeach entry contains parameter type and value which will be parsed using template then
 * @return - string query which will can be used in google drive
 */
const QString QueryEntry::getEntries(const QList<QueryEntry>& entries) {
QString result;
Q_FOREACH(const QueryEntry& entry,entries) {

    QMap<QueryFilter,QString> filters=QueryEntry::getFilters();
    QString rawQuery=filters[entry.type];
    Q_FOREACH(const QString& param,entry.parameters) {
    rawQuery=rawQuery.arg(param);
    }
    if (result.size()>0) {
    rawQuery=" and "+rawQuery;
    }
    result+=rawQuery;

    ;
}
return result;
}
