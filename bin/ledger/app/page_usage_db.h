// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PERIDOT_BIN_LEDGER_APP_PAGE_USAGE_DB_H_
#define PERIDOT_BIN_LEDGER_APP_PAGE_USAGE_DB_H_

#include <functional>
#include <memory>

#include <lib/callback/operation_serializer.h>

#include "lib/fxl/strings/concatenate.h"
#include "peridot/bin/ledger/app/page_utils.h"
#include "peridot/bin/ledger/app/types.h"
#include "peridot/bin/ledger/coroutine/coroutine.h"
#include "peridot/bin/ledger/environment/environment.h"
#include "peridot/bin/ledger/storage/impl/leveldb.h"
#include "peridot/bin/ledger/storage/public/db.h"
#include "peridot/bin/ledger/storage/public/iterator.h"
#include "peridot/bin/ledger/storage/public/types.h"

namespace ledger {

// |PageUsageDb| persists all information on page usage.
//
// Calls to |MarkPageOpened| and |MarkPageClosed| will update the underlying
// database in the order in which they are called.
//
// Rows in the underlying database are serialized as follows:
// Last usage row:
// - Key: "opened/<ledger_name><page_id>"
// - Value: "<timestamp>" or timestamp 0 for open pages
class PageUsageDb {
 public:
  PageUsageDb(Environment* environment, DetachedPath db_path);
  ~PageUsageDb();

  // Initializes the underlying database. Init should be called before any other
  // operation is performed.
  Status Init();

  // Marks the page with the given id as opened. |INTERNAL_ERROR| is returned if
  // the operation is interrupted.
  Status MarkPageOpened(coroutine::CoroutineHandler* handler,
                        fxl::StringView ledger_name,
                        storage::PageIdView page_id);

  // Marks the page with the given id as closed. |INTERNAL_ERROR| is returned if
  // the operation is interrupted.
  Status MarkPageClosed(coroutine::CoroutineHandler* handler,
                        fxl::StringView ledger_name,
                        storage::PageIdView page_id);

  // Marks the page with the given id as evicted. |INTERNAL_ERROR| is returned
  // if the operation is interrupted.
  Status MarkPageEvicted(coroutine::CoroutineHandler* handler,
                         fxl::StringView ledger_name,
                         storage::PageIdView page_id);

  // Marks all open pages as closed. |INTERNAL_ERROR| is returned if the
  // operation is interrupted.
  Status MarkAllPagesClosed(coroutine::CoroutineHandler* handler);

  // Updates |pages| to contain an iterator over all entries of page
  // information.
  Status GetPages(coroutine::CoroutineHandler* handler,
                  std::unique_ptr<storage::Iterator<const PageInfo>>* pages);

 private:
  // Inserts the given |key|-|value| pair in the underlying database.
  Status Put(coroutine::CoroutineHandler* handler, fxl::StringView key,
             fxl::StringView value);

  // Deletes the row with the given |key| in the underlying database.
  Status Delete(coroutine::CoroutineHandler* handler, fxl::StringView key);

  Environment* environment_;
  storage::LevelDb db_;

  // A serializer used for Put and Delete. Both these operations need to be
  // serialized to guarantee that consecutive calls to update the contents of a
  // single page (e.g. a page is opened and then closed) are written in |db_| in
  // the right order, i.e. the order in which they were called.
  callback::OperationSerializer serializer_;

  FXL_DISALLOW_COPY_AND_ASSIGN(PageUsageDb);
};

}  // namespace ledger

#endif  // PERIDOT_BIN_LEDGER_APP_PAGE_USAGE_DB_H_
