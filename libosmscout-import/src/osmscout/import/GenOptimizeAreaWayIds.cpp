/*
  This source is part of the libosmscout library
  Copyright (C) 2013  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <osmscout/import/GenOptimizeAreaWayIds.h>

#include <osmscout/util/FileScanner.h>
#include <osmscout/util/FileWriter.h>

#include <osmscout/DataFile.h>

namespace osmscout {

  std::string OptimizeAreaWayIdsGenerator::GetDescription() const
  {
    return "Optimize ids for areas and ways";
  }

  void OptimizeAreaWayIdsGenerator::SetNodeUsed(NodeUseMap& nodeUseMap,
                                                OSMId id)
  {
    PageId resolvedId=id-std::numeric_limits<Id>::min();
    PageId offset=resolvedId/16;

    NodeUseMap::iterator entry=nodeUseMap.find(offset);

    if (entry==nodeUseMap.end()) {
      entry=nodeUseMap.insert(std::make_pair(offset,0)).first;
    }

    uint32_t index=(resolvedId%16)*2;

    uint32_t data=entry->second;

    if (data & (1<<(index+1))) {
      // do nothing
    }
    else if (data & (1<<index)) {
      entry->second|=(1 << (index+1));
    }
    else {
      entry->second|=(1 << index);
    }
  }

  bool OptimizeAreaWayIdsGenerator::IsNodeUsedAtLeastTwice(const NodeUseMap& nodeUseMap,
                                                           OSMId id) const
  {
    PageId resolvedId=id-std::numeric_limits<Id>::min();
    PageId offset=resolvedId/16;

    NodeUseMap::const_iterator entry=nodeUseMap.find(offset);

    if (entry==nodeUseMap.end()) {
      return false;
    }

    uint32_t index=(resolvedId%16)*2+1;

    bool result=entry->second & (1 << index);

    return result;
  }

  bool OptimizeAreaWayIdsGenerator::ScanWayAreaIds(const ImportParameter& parameter,
                                                   Progress& progress,
                                                   NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    uint32_t    dataCount=0;

    progress.SetAction("Scanning ids from 'wayarea.tmp'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "wayarea.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetWayDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    for (size_t current=1; current<=dataCount; current++) {
      Id   id;
      Area data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(id)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      if (!data.Read(scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (std::vector<Area::Role>::const_iterator role=data.roles.begin();
           role!=data.roles.end();
           role++) {
        std::set<Id> nodeIds;

        for (std::vector<Id>::const_iterator id=role->ids.begin();
             id!=role->ids.end();
             id++) {
          if (nodeIds.find(*id)==nodeIds.end()) {
            SetNodeUsed(nodeUseMap,*id);

            nodeIds.insert(*id);
          }
        }
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::ScanRelAreaIds(const ImportParameter& parameter,
                                                   Progress& progress,
                                                   NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    uint32_t    dataCount=0;

    progress.SetAction("Scanning ids from 'relarea.tmp'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "relarea.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetWayDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    for (size_t current=1; current<=dataCount; current++) {
      Id   id;
      Area data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(id)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      if (!data.Read(scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (std::vector<Area::Role>::const_iterator role=data.roles.begin();
           role!=data.roles.end();
           role++) {
        std::set<Id> nodeIds;

        for (std::vector<Id>::const_iterator id=role->ids.begin();
             id!=role->ids.end();
             id++) {
          if (nodeIds.find(*id)==nodeIds.end()) {
            SetNodeUsed(nodeUseMap,*id);

            nodeIds.insert(*id);
          }
        }
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::ScanWayWayIds(const ImportParameter& parameter,
                                                  Progress& progress,
                                                  NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    uint32_t    dataCount=0;

    progress.SetAction("Scanning ids from 'wayway.tmp'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "wayway.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetWayDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    for (size_t current=1; current<=dataCount; current++) {
      Id  id;
      Way data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(id)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      if (!data.Read(scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      std::set<Id> nodeIds;

      for (std::vector<Id>::const_iterator id=data.ids.begin();
          id!=data.ids.end();
          id++) {
        if (nodeIds.find(*id)==nodeIds.end()) {
          SetNodeUsed(nodeUseMap,*id);

          nodeIds.insert(*id);
        }
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::CopyWayArea(const ImportParameter& parameter,
                                                Progress& progress,
                                                NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    FileWriter  writer;
    uint32_t    dataCount=0;

    progress.SetAction("Copy data from 'wayarea.tmp' to 'wayarea.dat'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "wayarea.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetWayDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    if (!writer.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                     "wayarea.dat"))) {
      progress.Error(std::string("Cannot create '")+writer.GetFilename()+"'");
      return false;
    }

    writer.Write(dataCount);

    for (size_t current=1; current<=dataCount; current++) {
      Id   id;
      Area data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(id)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      if (!data.Read(scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (std::vector<Area::Role>::iterator role=data.roles.begin();
           role!=data.roles.end();
           role++) {
        std::set<Id> nodeIds;

        for (std::vector<Id>::iterator id=role->ids.begin();
             id!=role->ids.end();
             id++) {
          if (!IsNodeUsedAtLeastTwice(nodeUseMap,*id)) {
            *id=0;
          }
        }
      }

      if (!writer.Write(id)) {
        progress.Error(std::string("Error while writing data id to file '")+
                       writer.GetFilename()+"'");

        return false;
      }

      if (!data.Write(writer)) {
        progress.Error(std::string("Error while writing data entry to file '")+
                       writer.GetFilename()+"'");

        return false;
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    if (!writer.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     writer.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::CopyRelArea(const ImportParameter& parameter,
                                                Progress& progress,
                                                NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    FileWriter  writer;
    uint32_t    dataCount=0;

    progress.SetAction("Copy data from 'relarea.tmp' to 'relarea.dat'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "relarea.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetWayDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    if (!writer.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                     "relarea.dat"))) {
      progress.Error(std::string("Cannot create '")+writer.GetFilename()+"'");
      return false;
    }

    writer.Write(dataCount);

    for (size_t current=1; current<=dataCount; current++) {
      Id   id;
      Area data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(id)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      if (!data.Read(scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (std::vector<Area::Role>::iterator role=data.roles.begin();
           role!=data.roles.end();
           role++) {
        std::set<Id> nodeIds;

        for (std::vector<Id>::iterator id=role->ids.begin();
             id!=role->ids.end();
             id++) {
          if (!IsNodeUsedAtLeastTwice(nodeUseMap,*id)) {
            *id=0;
          }
        }
      }

      if (!writer.Write(id)) {
        progress.Error(std::string("Error while writing data id to file '")+
                       writer.GetFilename()+"'");

        return false;
      }

      if (!data.Write(writer)) {
        progress.Error(std::string("Error while writing data entry to file '")+
                       writer.GetFilename()+"'");

        return false;
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    if (!writer.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     writer.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::CopyWayWay(const ImportParameter& parameter,
                                               Progress& progress,
                                               NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    FileWriter  writer;
    uint32_t    dataCount=0;

    progress.SetAction("Copy data from 'wayway.tmp' to 'wayway.dat'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "wayway.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetWayDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    if (!writer.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                     "wayway.dat"))) {
      progress.Error(std::string("Cannot create '")+writer.GetFilename()+"'");
      return false;
    }

    writer.Write(dataCount);

    for (size_t current=1; current<=dataCount; current++) {
      Id  id;
      Way data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(id)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      if (!data.Read(scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (std::vector<Id>::iterator id=data.ids.begin();
          id!=data.ids.end();
          id++) {
        if (!IsNodeUsedAtLeastTwice(nodeUseMap,*id)) {
          *id=0;
        }
      }

      if (!writer.Write(id)) {
        progress.Error(std::string("Error while writing data id to file '")+
                       writer.GetFilename()+"'");

        return false;
      }

      if (!data.Write(writer)) {
        progress.Error(std::string("Error while writing data entry to file '")+
                       writer.GetFilename()+"'");

        return false;
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    if (!writer.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     writer.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::Import(const ImportParameter& parameter,
                                           Progress& progress,
                                           const TypeConfig& typeConfig)
  {
    progress.SetAction("Optimize ids for areas and ways");

    NodeUseMap nodeUseMap;

    if (!ScanWayAreaIds(parameter,
                        progress,
                        nodeUseMap)) {
      return false;
    }

    if (!ScanRelAreaIds(parameter,
                        progress,
                        nodeUseMap)) {
      return false;
    }

    if (!ScanWayWayIds(parameter,
                       progress,
                       nodeUseMap)) {
      return false;
    }

    if (!CopyWayArea(parameter,
                     progress,
                     nodeUseMap)) {
      return false;
    }

    if (!CopyRelArea(parameter,
                     progress,
                     nodeUseMap)) {
      return false;
    }

    if (!CopyWayWay(parameter,
                    progress,
                    nodeUseMap)) {
      return false;
    }

    return true;
  }
}

