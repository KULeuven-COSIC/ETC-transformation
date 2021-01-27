{-# LANGUAGE OverloadedStrings #-}
import qualified Data.ByteString.Lazy.Char8 as BS
import qualified Data.IntSet as IntSet
import Data.IntSet (IntSet)
import Data.Char
import qualified Data.List as List
import System.Environment
import Text.Printf

import qualified Data.Map.Strict as Map
import Data.Map.Strict (Map)

data Line = ILoad Int Int
          | Store Int Int
  deriving (Eq,Ord,Show)

parse :: BS.ByteString -> [Line]
parse s =
    let s1 = BS.dropWhile isSpace s
        s1x = BS.take 1 s1
        s2 = BS.dropWhile isSpace $ BS.drop 1 s1
        s3 = BS.takeWhile isHexDigit s2
        s4 = BS.drop (BS.length s3) s2
        s4x = BS.take 1 s4
        s5 = BS.drop 1 s4
    in case (s1x,reads $ "0x"++BS.unpack s3, s4x, reads $ BS.unpack s5) of
        ("I",[(addr,_)],",",[(n,_)]) -> [ILoad addr n]
        ("S",[(addr,_)],",",[(n,_)]) -> [Store addr n]
        _ -> []

adjSet :: IntSet -> Line -> IntSet
adjSet x ILoad{} = x
adjSet x (Store a s) = IntSet.union x $ IntSet.fromList [a..a+s-1]

probe :: Map Int b -> Maybe Int -> Maybe Int -> [Line] -> [(b,Int,IntSet)]
probe m _ _ [] = []
probe m (Just a') (Just frame) (ILoad a b:is)
  | Just name <- Map.lookup a m =
    let isFinish (ILoad a'' b') = a' == a''
        isFinish _ = False
        isLessFrame (Store addr' _) = addr' < frame
        isLessFrame _ = False
        ret = (name,frame,List.foldl' adjSet IntSet.empty $ {-filter isLessFrame $ -} List.takeWhile (not . isFinish) is)
    in ret : probe m Nothing Nothing (List.dropWhile (not . isFinish) is)
probe m _ _ (ILoad a b:is) = probe m (Just (a+b)) Nothing is
probe m praddr _ (Store a _:is) = probe m praddr (Just a) is

describe :: Int -> IntSet -> (Int,Int,Int,Int)
describe frame mem = (stotal,saccessed,oaccessed,haccessed)
  where huge = 2^20
        splitGe k s = let (a,b,c) = IntSet.splitMember k s
            in if b then (a,IntSet.insert k c)
            else (a,c)
            
        (stackHeap,outHeap) = splitGe frame mem
        (heap1,stack) = splitGe (frame-huge) stackHeap
        (out,heap2) = splitGe (frame+huge) outHeap
        heap = IntSet.union heap1 heap2
        
        haccessed = IntSet.size heap
        oaccessed = IntSet.size out
        saccessed = IntSet.size stack
        stotal = if IntSet.null stack then 0
                    else IntSet.findMax stack + 1 - IntSet.findMin stack

describePrint :: String -> Int -> IntSet -> String
describePrint name frame mem = printf "%40s: stacktotal:%6d, padding:%6d, out:%6d, heap:%6d"
    name stotal (stotal-saccessed) oaccessed haccessed
    where (stotal,saccessed,oaccessed,haccessed) = describe frame mem

main = do
    args <- getArgs
    dat <- BS.getContents
    let mkmap (addr:"T":name:xs)
            | "test" `List.isSuffixOf` name = mkmap xs
            | otherwise = (read $ "0x"++addr,name) : mkmap xs
        mkmap [] = []
        ret = probe (Map.fromList $ mkmap args)
                    Nothing Nothing
                    (parse =<< BS.lines dat)
    sequence_ [
        putStrLn $ describePrint name frame l
        | (name,frame,l) <- ret
        ]
        
            
        
    