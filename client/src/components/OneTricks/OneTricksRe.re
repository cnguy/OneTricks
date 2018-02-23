type sort =
  | REGION
  | RANK
  | NAME
  | WINS
  | LOSSES
  | WINRATE
  | NONE;

type page =
  | HOME
  | PLAYERS_VIEW
  | PLAYER;

type action =
  /* router actions */
  | ShowHome
  | ShowPlayersViewForChampion(string)
  | ShowPlayer(int)
  /* misc actions */
  | ResetSearchKey
  | SetSearchKey(string)
  | SetSortKey(sort)
  | ToggleMerge
  | ToggleAdvancedFilter
  | Nothing;

/*
     withState('all', 'setAll', {}), // LET JS HANDLE THIS FOR NOW
     withState('region', 'setRegion', 'all'),
     withState('regions', 'setRegions', DEFAULT_REGIONS.slice()),
     withState('champ', 'setChampionName', ''), // Shouldn't need this with URLs
     withState('players', 'setPlayers', []), // Shouldn't need this with URLs
     withState('showChamps', 'setShowChamps', true),
     withState('imagesLoaded', 'setImagesLoaded', false),
 */
type championPane = {
  isMultipleRegionFilterOn: bool,
  searchKey: string
};

type misc = {
  areChampionPanesMerged: bool,
  region: string,
  regions: array(string),
  areImagesLoaded: bool
};

type playersView = {
  sortKey: sort,
  shouldSortReverse: bool,
  currentChampion: string
};

type router = {page};

type state = {
  championPane,
  misc,
  playersView,
  router
};

let component = ReasonReact.reducerComponent("OneTricksRe");

let urlToShownPage = (path, search) =>
  switch (path, search) {
  | ([""], "") => HOME
  | (["player", id], "") => PLAYER
  | (["champions", name], "") => PLAYERS_VIEW
  | _ => HOME
  };

let urlToAction = (path, search) =>
  switch (path, search) {
  | ([""], "") => ShowHome
  | (["player", id], "") => ShowPlayer(int_of_string(id))
  | (["champions", name], "") =>
    Js.log("sending");
    ShowPlayersViewForChampion(name);
  | _ => ShowHome
  };

let make = (~allOneTricks, ~getPlayers, ~areImagesLoaded, _children) => {
  ...component,
  initialState: () => {
    championPane: {
      isMultipleRegionFilterOn: false,
      searchKey: ""
    },
    misc: {
      areChampionPanesMerged: false,
      region: "all",
      regions: Array.copy(Constants.regions),
      areImagesLoaded: false
    },
    playersView: {
      sortKey: WINRATE,
      shouldSortReverse: false,
      currentChampion:
        switch (
          ReasonReact.Router.dangerouslyGetInitialUrl().path: list(string)
        ) {
        | ["champions", name] => name
        | _ => ""
        }
    },
    router: {
      page:
        urlToShownPage(
          ReasonReact.Router.dangerouslyGetInitialUrl().path,
          ReasonReact.Router.dangerouslyGetInitialUrl().search
        )
    }
  },
  reducer: (action, state) =>
    switch action {
    | Nothing =>
      Js.log("Nothing will happen");
      ReasonReact.Update(state);
    | ShowHome => ReasonReact.Update({
                    ...state,
                    router: {
                      page: HOME
                    }
                  })
    | ShowPlayersViewForChampion(name) =>
      ReasonReact.Update({
        ...state,
        router: {
          page: PLAYERS_VIEW
        },
        playersView: {
          ...state.playersView,
          currentChampion: name
        }
      })
    | ShowPlayer(summonerID) =>
      Js.log(summonerID);
      ReasonReact.Update({
        ...state,
        router: {
          page: PLAYER
        }
      });
    | SetSortKey(sortKey) =>
      ReasonReact.Update({
        ...state,
        playersView: {
          ...state.playersView,
          sortKey,
          shouldSortReverse:
            if (sortKey == state.playersView.sortKey) {
              ! state.playersView.shouldSortReverse;
            } else {
              false;
            }
        }
      })
    | _ => ReasonReact.Update(state)
    },
  subscriptions: self => [
    Sub(
      () =>
        ReasonReact.Router.watchUrl(url => {
          Js.log("watching for URL");
          self.send(urlToAction(url.path, url.search));
        }),
      /*
       /* Here we should figure out how to parse the region. */
       let splitPoint = 7; /* length of word `regions` */
       let pre = String.sub(csvRegions, 0, splitPoint);
       let post = Js.String.substr(splitPoint + 1, csvRegions);
       if (pre === "regions") {
         let splitted = Js.String.splitByRe(Js.Re.fromString(","), post);
         Js.log("split");
         Js.log(splitted);
       };
       Js.log(csvRegions);
       self.send(Nothing);*/
      ReasonReact.Router.unwatchUrl
    )
  ],
  render: self => {
    Js.log(self.state);
    let tempOnSort = str =>
      self.send(
        SetSortKey(
          switch str {
          | "REGION" => REGION
          | "RANK" => RANK
          | "NAME" => NAME
          | "WINS" => WINS
          | "LOSSES" => LOSSES
          | "WINRATE" => WINRATE
          | "NONE" => NONE
          | _ => NONE
          }
        )
      );
    let sortKeyToStr = sortKey =>
      switch sortKey {
      | REGION => "REGION"
      | RANK => "RANK"
      | NAME => "NAME"
      | WINS => "WINS"
      | LOSSES => "LOSSES"
      | WINRATE => "WINRATE"
      | NONE => "NONE"
      };
    let mainComponent =
      switch self.state.router.page {
      | PLAYERS_VIEW =>
        let currentChampion: string = self.state.playersView.currentChampion;
        let target =
          List.filter(
            (el: Types.oneTrick) =>
              Utils.parseChampionNameFromRoute(el##champion)
              === currentChampion,
            Array.to_list(allOneTricks)
          );
        let players: array(Types.player) =
          if (List.length(target) === 1) {
            Array.of_list(target)[0]##players;
          } else {
            [||];
          };
        <PlayersView
          players
          goBack=(_event => ReasonReact.Router.push(""))
          champ=self.state.playersView.currentChampion
          show=true
          onSort=tempOnSort
          sortKey=(sortKeyToStr(self.state.playersView.sortKey))
          sortReverse=self.state.playersView.shouldSortReverse
        />;
      | _ => ReasonReact.nullElement
      };
    <div className="one-tricks-re">
      <button className="router-test">
        (
          ReasonReact.stringToElement(
            switch self.state.router.page {
            | HOME => "Home"
            | PLAYERS_VIEW => "Players View"
            | PLAYER => "Player!"
            }
          )
        )
      </button>
      mainComponent
    </div>;
    /*
     if (areImagesLoaded) {
       <div className="OneTricksRe">
         <button onClick=(_event => ReasonReact.Router.push(""))>
           (ReasonReact.stringToElement("Simple Button"))
         </button>
         <button
           onClick=(_event => ReasonReact.Router.push("/champions/shaco"))>
           (ReasonReact.stringToElement("Test"))
         </button>
         <button onClick=(_event => ReasonReact.Router.push("/player/50"))>
           (ReasonReact.stringToElement("Some Player Button"))
         </button>
       </div>;
     } else {
       ReasonReact.nullElement;
     };*/
  }
};

let default =
  ReasonReact.wrapReasonForJs(~component, jsProps =>
    make(
      ~allOneTricks=jsProps##allOneTricks,
      ~getPlayers=jsProps##getPlayers,
      ~areImagesLoaded=jsProps##areImagesLoaded,
      jsProps##children
    )
  );