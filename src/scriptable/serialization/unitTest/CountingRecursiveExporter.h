#ifndef __COUNTINGRECURSIVEEXPORTER_H__
#define __COUNTINGRECURSIVEEXPORTER_H__

#include "../AbstractRecursiveExporter.h"

/**
 * An CountingRecursiveExporter is a mock exporter which counts how
 * many times its methods are called to allow verification of the
 * AbstractRecursiveExporter behavior.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class CountingRecursiveExporter : public AbstractRecursiveExporter {
  public:
    CountingRecursiveExporter() {
        reset();
    }

    void reset() {
        ciWriteHeaderInvocations = 0;
        ciStartObjectInvocations = 0;
        ciFinishObjectPropertiesInvocations = 0;
        ciFinishObjectInvocations = 0;
        ciStartPropertyInvocations = 0;
        ciFinishPropertyInvocations = 0;
        ciHandleRelationshipInvocations = 0;
        ciWriteTrailerInvocations = 0;
        ciWriteSerializationBasePropertyInvocations = 0;
    }

    int getWriteHeaderInvocations() { return ciWriteHeaderInvocations; };
    int getStartObjectInvocations() { return ciStartObjectInvocations; };
    int getFinishObjectPropertiesInvocations() { return ciFinishObjectPropertiesInvocations; };
    int getFinishObjectInvocations() { return ciFinishObjectInvocations; };
    int getStartPropertyInvocations() { return ciStartPropertyInvocations; };
    int getFinishPropertyInvocations() { return ciFinishPropertyInvocations; };
    int getWriteTrailerInvocations() { return ciWriteTrailerInvocations; };
    
  protected:
    virtual void openOutput() {}
    virtual void writeHeader(Exportable& exp) { ciWriteHeaderInvocations++; }
    virtual void startObject(Exportable& exp, Exportable& expParent, const string& strPrefix) { ciStartObjectInvocations++; };
    virtual void finishObjectProperties(Exportable& exp, const string& strPrefix) { ciFinishObjectPropertiesInvocations++; };
    virtual void finishObject(Exportable& exp, const string& strPrefix) { ciFinishObjectInvocations++; };
    virtual void writeSerializationBaseProperty() { ciWriteSerializationBasePropertyInvocations++; }
    virtual void startProperty(const string& strName,
                               const string& strValue,
                               const string& strPrefix) { ciStartPropertyInvocations++; };
    virtual void finishProperty(const string& strPrefix) { ciFinishPropertyInvocations++; };
    virtual string getSpecialPropertyValue(Exportable& exp, int propId){ return ""; }  
    virtual void handleRelationship(Exportable& exp, 
                                    const string& strType,
                                    bool bSourceToTarget,
                                    bool last,
                                    const string& strPrefix) { ciHandleRelationshipInvocations++; }
    virtual void writeTrailer() { ciWriteTrailerInvocations++; }
    virtual void closeOutput() {}
    virtual string incrementPrefix(string& strPrefix) { return strPrefix; }
  private:
    int ciWriteHeaderInvocations;
    int ciStartObjectInvocations;
    int ciFinishObjectPropertiesInvocations;
    int ciFinishObjectInvocations;
    int ciStartPropertyInvocations;
    int ciFinishPropertyInvocations;
    int ciHandleRelationshipInvocations;
    int ciWriteTrailerInvocations;
    int ciWriteSerializationBasePropertyInvocations;
};

#endif

